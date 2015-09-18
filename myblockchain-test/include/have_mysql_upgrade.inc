--require r/have_myblockchain_upgrade.result
--disable_query_log
select LENGTH("$MYBLOCKCHAIN_UPGRADE")>0 as have_myblockchain_upgrade;
--enable_query_log
