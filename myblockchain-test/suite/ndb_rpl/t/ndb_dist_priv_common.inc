#
# Create test objects
#
--connection server1
CREATE DATABASE myblockchaintest1;
CREATE DATABASE myblockchaintest2;

use myblockchaintest1;
CREATE TABLE t1 (
  a INT PRIMARY KEY,
  b char(20)
) engine = ndb;
INSERT INTO t1 VALUES (1, "row1"), (2, "row2");

use myblockchaintest2;
CREATE TABLE t2 (
  a INT PRIMARY KEY,
  b char(20)
) engine = ndb;
INSERT INTO t2 VALUES (3, "row4"), (4, "row4");

CREATE USER 'testuser1'@'localhost';
--connect(user1,127.0.0.1,testuser1,,test,$MASTER_MYPORT)

#####################################################
# SET PASSWORD
#

--connection user1
let $cmd= SET PASSWORD = 'cleartext password';
let $chk= SELECT Authentication_string FROM myblockchain.user
          WHERE Host='localhost' AND User='testuser1';
--source ndb_dist_priv_check.inc

--connection user1
let $encrypted_password_hash = 'new cleartext password';
let $cmd=SET PASSWORD = $encrypted_password_hash;
let $chk= SELECT Authentication_string FROM myblockchain.user
          WHERE Host='localhost' AND User='testuser1';
--source ndb_dist_priv_check.inc

--connection server2
let $cmd= SET PASSWORD FOR testuser1@localhost= 'pass_for_user1';
let $chk= SELECT Authentication_string FROM myblockchain.user
          WHERE Host='localhost' AND User='testuser1';
--source ndb_dist_priv_check.inc

--connection server2
let $encrypted_password_hash = 'another pass';
let $cmd=SET PASSWORD FOR testuser1@localhost = $encrypted_password_hash;
let $chk= SELECT Authentication_string FROM myblockchain.user
          WHERE Host='localhost' AND User='testuser1';
--source ndb_dist_priv_check.inc

# Try to set password for non existing user, fails and should not be
# replicted to slave
--connection server2
let $cmd_error= ER_PASSWORD_NO_MATCH;
let $cmd= SET PASSWORD FOR no_such_user@localhost= 'new pass';
let $chk= SELECT Authentication_string FROM myblockchain.user
          WHERE Host='localhost' AND User='no_such_user';
--source ndb_dist_priv_check.inc

#####################################################
# GRANT/REVOKE
#

# Global privileges
--connection server2
let $cmd= GRANT SELECT ON *.* TO testuser1@localhost;
let $chk= SELECT Select_priv='Y' FROM myblockchain.user
          WHERE Host='localhost' AND User='testuser1';
--source ndb_dist_priv_check.inc

--connection server2
let $cmd= REVOKE SELECT ON *.* FROM testuser1@localhost;
let $chk= SELECT Select_priv='N' FROM myblockchain.user
          WHERE Host='localhost' AND User='testuser1';
--source ndb_dist_priv_check.inc

# Database privileges(using current db)
--connection server2
let $cmd= GRANT SELECT ON * TO testuser1@localhost;
let $chk= SELECT Select_priv='Y' FROM myblockchain.db
          WHERE Host='localhost' AND User='testuser1' AND Db='test';
--source ndb_dist_priv_check.inc

--connection server2
let $cmd= REVOKE SELECT ON * FROM testuser1@localhost;
let $chk= SELECT Select_priv='N' FROM myblockchain.db
          WHERE Host='localhost' AND User='testuser1' AND Db='test';
--source ndb_dist_priv_check.inc

# Database privileges
--connection server2
let $cmd= GRANT SELECT ON myblockchaintest2.* TO testuser1@localhost;
let $chk= SELECT Select_priv='Y' FROM myblockchain.db
          WHERE Host='localhost' AND User='testuser1' AND Db='myblockchaintest2';
--source ndb_dist_priv_check.inc

--connection server2
let $cmd= REVOKE SELECT ON myblockchaintest2.* FROM testuser1@localhost;
let $chk= SELECT Select_priv='N' FROM myblockchain.db
          WHERE Host='localhost' AND User='testuser1' AND Db='myblockchaintest2';
--source ndb_dist_priv_check.inc

# Table privileges
--connection server2
let $cmd= GRANT SELECT ON myblockchaintest2.t2 TO testuser1@localhost;
let $chk= SELECT Table_priv='Select' FROM myblockchain.tables_priv
          WHERE Host='localhost' AND User='testuser1' AND
          Db='myblockchaintest2' AND Table_name='t2';
--source ndb_dist_priv_check.inc

--connection server2
let $cmd= REVOKE SELECT ON myblockchaintest2.t2 FROM testuser1@localhost;
let $chk= SELECT Count(Table_priv) FROM myblockchain.tables_priv
          WHERE Host='localhost' AND User='testuser1' AND
                Db='myblockchaintest2' AND Table_name='t2';
--source ndb_dist_priv_check.inc

# Column privileges
--connection server2
let $cmd= GRANT SELECT(a,b), UPDATE(a) ON myblockchaintest2.t2 TO testuser1@localhost;
let $chk= SELECT COUNT(Column_priv) FROM myblockchain.columns_priv
          WHERE Host='localhost' AND User='testuser1' AND
                Db='myblockchaintest2' AND Table_name='t2' AND
                (Column_name='a' or Column_name='b');
--source ndb_dist_priv_check.inc

--connection server2
let $cmd= REVOKE SELECT(a,b), UPDATE(a) ON myblockchaintest2.t2 FROM testuser1@localhost;
let $chk= SELECT Count(Column_priv) FROM myblockchain.columns_priv
          WHERE Host='localhost' AND User='testuser1' AND
                Db='myblockchaintest2' AND Table_name='t2' AND
                (Column_name='a' or Column_name='b');
--source ndb_dist_priv_check.inc

--connection server2
let $cmd_error= ER_BAD_FIELD_ERROR; # Unknown column 'c' in 't2'
let $cmd= GRANT SELECT(c,b), UPDATE(a) ON myblockchaintest2.t2 TO testuser1@localhost;
let $chk= SELECT COUNT(Column_priv) FROM myblockchain.columns_priv
          WHERE Host='localhost' AND User='testuser1' AND
                Db='myblockchaintest2' AND Table_name='t2' AND
                (Column_name='a' or Column_name='b');
--source ndb_dist_priv_check.inc

#####################################################
# CREATE/DROP USER
#

--connection server2
let $cmd= CREATE USER testuser2@localhost IDENTIFIED BY 'pass4';
let $chk= SELECT COUNT(*)=1 FROM myblockchain.user
          WHERE Host='localhost' AND User='testuser2';
--source ndb_dist_priv_check.inc

--connection server2
let $cmd= DROP USER testuser2@localhost;
let $chk= SELECT COUNT(*) FROM myblockchain.user
          WHERE Host='localhost' AND User='testuser2';
--source ndb_dist_priv_check.inc


#####################################################
# ALTER USER ... PASSWORD EXPIRE
#  - new in 5.6.6
#

--connection server2
let $cmd= ALTER USER testuser1@localhost PASSWORD EXPIRE;
let $chk= SELECT Password_expired='Y' FROM myblockchain.user
          WHERE Host='localhost' AND User='testuser1';
--source ndb_dist_priv_check.inc

--connection server2
let $cmd= SET PASSWORD FOR testuser1@localhost= 'pass_for_user1';
let $chk= SELECT Authentication_string FROM myblockchain.user
          WHERE Host='localhost' AND User='testuser1';
--source ndb_dist_priv_check.inc

#####################################################
# GRANT/REVOKE with DATABASE() being NULL
#
#

--connection server2
CREATE DATABASE myblockchaintest3;
USE myblockchaintest3;
SELECT DATABASE();
DROP DATABASE myblockchaintest3;
SELECT DATABASE(); # It's NULL now

let $cmd= GRANT ALL ON *.* TO testuser1@localhost;
let $chk= SELECT Select_priv='Y' FROM myblockchain.user
          WHERE Host='localhost' AND User='testuser1';
--source ndb_dist_priv_check.inc

--connection server2
let $cmd= REVOKE ALL ON *.* FROM testuser1@localhost;
let $chk= SELECT Select_priv='N' FROM myblockchain.user
          WHERE Host='localhost' AND User='testuser1';
--source ndb_dist_priv_check.inc


# Cleanup test objects
--connection server1
DROP USER 'testuser1'@'localhost';
DROP TABLE myblockchaintest1.t1, myblockchaintest2.t2;
DROP DATABASE myblockchaintest1;
DROP DATABASE myblockchaintest2;
