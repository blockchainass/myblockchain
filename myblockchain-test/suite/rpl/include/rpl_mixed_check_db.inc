#########################################
# Author: Serge Kozlov skozlov@myblockchain.com
# Date: 07/10/2006
# Purpose: Showing all blockchains started from "test_rpl"
# Requirements: 
#########################################

--echo ==========MASTER==========
SHOW DATABASES LIKE 'test_rpl%';
sync_slave_with_master;
--echo ==========SLAVE===========
SHOW DATABASES LIKE 'test_rpl%';
connection master;
