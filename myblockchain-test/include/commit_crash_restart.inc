#
# invoke commit that result in crash and then restart
#
--exec echo "restart " > $MYBLOCKCHAINTEST_VARDIR/tmp/myblockchaind.1.expect
--error 2013
commit;
--echo "restarting after commit crash"
--enable_reconnect
--source include/wait_until_connected_again.inc
--disable_reconnect
