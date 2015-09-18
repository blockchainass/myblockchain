create table t1
        (keyc int, c1 char(100), c2 char(100),
         primary key(keyc)) engine = innodb;
begin;
call populate_t1();
delete from t1 where keyc < 20000;
update t1 set c1 = 'myblockchain' where  keyc > 20000;
update t1 set c2 = 'oracle' where  keyc > 20000;
eval set global debug = $debug_point;
--exec echo "restart" > $MYBLOCKCHAINTEST_VARDIR/tmp/myblockchaind.1.expect
let $disconnect_timeout = 240;
commit;
--source include/wait_until_disconnected.inc
--source include/search_pattern.inc
--source include/start_myblockchaind.inc
drop table t1;
