-- Copyright (c) 2011, 2013, Oracle and/or its affiliates. All rights reserved.
--
-- This program is free software; you can redistribute it and/or modify
-- it under the terms of the GNU General Public License as published by
-- the Free Software Foundation; version 2 of the License.
--
-- This program is distributed in the hope that it will be useful,
-- but WITHOUT ANY WARRANTY; without even the implied warranty of
-- MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
-- GNU General Public License for more details.
--
-- You should have received a copy of the GNU General Public License
-- along with this program; if not, write to the Free Software
-- Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA

delimiter |

drop function if exists myblockchain.myblockchain_cluster_privileges_are_distributed|
drop procedure if exists myblockchain.myblockchain_cluster_backup_privileges|
drop procedure if exists myblockchain.myblockchain_cluster_move_grant_tables|
drop procedure if exists myblockchain.myblockchain_cluster_restore_privileges_from_local|
drop procedure if exists myblockchain.myblockchain_cluster_restore_privileges|
drop procedure if exists myblockchain.myblockchain_cluster_restore_local_privileges|
drop procedure if exists myblockchain.myblockchain_cluster_move_privileges|

 -- Count number of privilege tables in NDB, require
 -- all the tables to be in NDB in order to return "true"
create function myblockchain.myblockchain_cluster_privileges_are_distributed()
returns bool
reads sql data
begin
 declare distributed bool default 0;

 select COUNT(table_name) = 6
   into distributed
     from information_schema.tables
       where table_schema = "myblockchain" and
             table_name IN ("user", "db", "tables_priv",
                            "columns_priv", "procs_priv",
                            "proxies_priv") and
             table_type = 'BASE TABLE' and
             engine = 'NDBCLUSTER';

 return distributed;
end|

create procedure myblockchain.myblockchain_cluster_backup_privileges()
begin
 declare distributed_privileges bool default 0;
 declare first_backup bool default 1;
 declare first_distributed_backup bool default 1;
 select myblockchain.myblockchain_cluster_privileges_are_distributed()
   into distributed_privileges;
 select 0 into first_backup
   from information_schema.tables
     where table_schema = "myblockchain" and table_name = "user_backup";
 select 0 into first_distributed_backup
   from information_schema.tables
     where table_schema = "myblockchain" and table_name = "ndb_user_backup";
 if first_backup = 1 then
   create table if not exists myblockchain.user_backup
     like myblockchain.user;
   create table if not exists myblockchain.db_backup
     like myblockchain.db;
   create table if not exists myblockchain.tables_priv_backup
     like myblockchain.tables_priv;
   create table if not exists myblockchain.columns_priv_backup
     like myblockchain.columns_priv;
   create table if not exists myblockchain.procs_priv_backup
     like myblockchain.procs_priv;
   create table if not exists myblockchain.proxies_priv_backup
     like myblockchain.proxies_priv;
   if distributed_privileges = 1 then
     alter table myblockchain.user_backup engine = myisam;
     alter table myblockchain.db_backup engine = myisam;
     alter table myblockchain.tables_priv_backup engine = myisam;
     alter table myblockchain.columns_priv_backup engine = myisam;
     alter table myblockchain.procs_priv_backup engine = myisam;
     alter table myblockchain.proxies_priv_backup engine = myisam;
   end if;
 else
   truncate myblockchain.user_backup;
   truncate myblockchain.db_backup;
   truncate myblockchain.tables_priv_backup;
   truncate myblockchain.columns_priv_backup;
   truncate myblockchain.procs_priv_backup;
   truncate myblockchain.proxies_priv_backup;
 end if;
 if first_distributed_backup = 1 then
   create table if not exists myblockchain.ndb_user_backup
     like myblockchain.user;
   create table if not exists myblockchain.ndb_db_backup
     like myblockchain.db;
   create table if not exists myblockchain.ndb_tables_priv_backup
     like myblockchain.tables_priv;
   create table if not exists myblockchain.ndb_columns_priv_backup
     like myblockchain.columns_priv;
   create table if not exists myblockchain.ndb_procs_priv_backup
     like myblockchain.procs_priv;
   create table if not exists myblockchain.ndb_proxies_priv_backup
     like myblockchain.proxies_priv;

   if distributed_privileges = 0 then
     alter table myblockchain.ndb_user_backup engine = ndbcluster;
     alter table myblockchain.ndb_db_backup engine = ndbcluster;
     alter table myblockchain.ndb_tables_priv_backup engine = ndbcluster;
     alter table myblockchain.ndb_columns_priv_backup engine = ndbcluster;
     alter table myblockchain.ndb_procs_priv_backup engine = ndbcluster;
     alter table myblockchain.ndb_proxies_priv_backup engine = ndbcluster;
   end if;
 else
   truncate myblockchain.ndb_user_backup;
   truncate myblockchain.ndb_db_backup;
   truncate myblockchain.ndb_tables_priv_backup;
   truncate myblockchain.ndb_columns_priv_backup;
   truncate myblockchain.ndb_procs_priv_backup;
   truncate myblockchain.ndb_proxies_priv_backup;
 end if;
 insert into myblockchain.user_backup select * from myblockchain.user;
 insert into myblockchain.db_backup select * from myblockchain.db;
 insert into myblockchain.tables_priv_backup select * from myblockchain.tables_priv;
 insert into myblockchain.columns_priv_backup select * from myblockchain.columns_priv;
 insert into myblockchain.procs_priv_backup select * from myblockchain.procs_priv;
 insert into myblockchain.proxies_priv_backup select * from myblockchain.proxies_priv;

 insert into myblockchain.ndb_user_backup select * from myblockchain.user;
 insert into myblockchain.ndb_db_backup select * from myblockchain.db;
 insert into myblockchain.ndb_tables_priv_backup select * from myblockchain.tables_priv;
 insert into myblockchain.ndb_columns_priv_backup select * from myblockchain.columns_priv;
 insert into myblockchain.ndb_procs_priv_backup select * from myblockchain.procs_priv;
 insert into myblockchain.ndb_proxies_priv_backup select * from myblockchain.proxies_priv;
end|

create procedure myblockchain.myblockchain_cluster_restore_privileges_from_local()
begin
 declare local_backup bool default 0;
 select 1 into local_backup
   from information_schema.tables
    where table_schema = "myblockchain" and table_name = "user_backup";
 if local_backup = 1 then
   create table if not exists myblockchain.user
     like myblockchain.user_backup;
   create table if not exists myblockchain.db
     like myblockchain.db_backup;
   create table if not exists myblockchain.tables_priv
     like myblockchain.tables_priv_backup;
   create table if not exists myblockchain.columns_priv
     like myblockchain.columns_priv_backup;
   create table if not exists myblockchain.procs_priv
     like myblockchain.procs_priv_backup;
   create table if not exists myblockchain.proxies_priv
     like myblockchain.proxies_priv_backup;
   delete from myblockchain.user;
   insert into myblockchain.user select * from myblockchain.user_backup;
   delete from myblockchain.db;
   insert into myblockchain.db select * from myblockchain.db_backup;
   delete from myblockchain.tables_priv;
   insert into myblockchain.tables_priv select * from myblockchain.tables_priv_backup;
   delete from myblockchain.columns_priv;
   insert into myblockchain.columns_priv select * from myblockchain.columns_priv_backup;
   delete from myblockchain.procs_priv;
   insert into myblockchain.procs_priv select * from myblockchain.procs_priv_backup;
   delete from myblockchain.proxies_priv;
   insert into myblockchain.proxies_priv select * from myblockchain.proxies_priv_backup;
 end if;
end|

create procedure myblockchain.myblockchain_cluster_restore_privileges()
begin
 declare distributed_backup bool default 0;
 select 1 into distributed_backup
   from information_schema.tables
     where table_schema = "myblockchain" and table_name = "ndb_user_backup";
 if distributed_backup = 1 then
   flush tables;
   create table if not exists myblockchain.user
     like myblockchain.ndb_user_backup;
   create table if not exists myblockchain.db
     like myblockchain.ndb_db_backup;
   create table if not exists myblockchain.tables_priv
     like myblockchain.ndb_tables_priv_backup;
   create table if not exists myblockchain.columns_priv
     like myblockchain.ndb_columns_priv_backup;
   create table if not exists myblockchain.procs_priv
     like myblockchain.ndb_procs_priv_backup;
   create table if not exists myblockchain.proxies_priv
     like myblockchain.ndb_proxies_priv_backup;
   delete from myblockchain.user;
   insert into myblockchain.user
     select * from myblockchain.ndb_user_backup;
   delete from myblockchain.db;
   insert into myblockchain.db
     select * from myblockchain.ndb_db_backup;
   delete from myblockchain.tables_priv;
   insert into myblockchain.tables_priv
     select * from myblockchain.ndb_tables_priv_backup;
   delete from myblockchain.columns_priv;
   insert into myblockchain.columns_priv
     select * from myblockchain.ndb_columns_priv_backup;
   delete from myblockchain.procs_priv;
   insert into myblockchain.procs_priv
     select * from myblockchain.ndb_procs_priv_backup;
   delete from myblockchain.proxies_priv;
   insert into myblockchain.proxies_priv
     select * from myblockchain.ndb_proxies_priv_backup;
 else
   call myblockchain_cluster_restore_privileges_from_local();
 end if;
end|

create procedure myblockchain.myblockchain_cluster_restore_local_privileges()
begin
 declare distributed_privileges bool default 0;
 select myblockchain.myblockchain_cluster_privileges_are_distributed()
   into distributed_privileges;
 if distributed_privileges = 1 then
  begin
    drop table myblockchain.user;
    drop table myblockchain.db;
    drop table myblockchain.tables_priv;
    drop table myblockchain.columns_priv;
    drop table myblockchain.procs_priv;
    drop table myblockchain.proxies_priv;
  end;
 end if;
 call myblockchain_cluster_restore_privileges_from_local();
end|

create procedure myblockchain.myblockchain_cluster_move_grant_tables()
begin
 declare distributed_privileges bool default 0;
 declare revert bool default 0;
 select myblockchain.myblockchain_cluster_privileges_are_distributed()
   into distributed_privileges;
 if distributed_privileges = 0 then
  begin
   declare exit handler for sqlexception set revert = 1;
   alter table myblockchain.user engine = ndb;
   alter table myblockchain.db engine = ndb;
   alter table myblockchain.tables_priv engine = ndb;
   alter table myblockchain.columns_priv engine = ndb;
   alter table myblockchain.procs_priv engine = ndb;
   alter table myblockchain.proxies_priv engine = ndb;
  end;
 end if;
 if revert = 1 then
   call myblockchain_cluster_restore_privileges();
 end if;
end|

create procedure myblockchain.myblockchain_cluster_move_privileges()
begin
 call myblockchain_cluster_backup_privileges();
 call myblockchain_cluster_move_grant_tables();
end|

delimiter ;

