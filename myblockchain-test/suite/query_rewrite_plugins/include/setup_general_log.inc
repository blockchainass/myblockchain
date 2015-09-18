SET @old_log_output=          @@global.log_output;
SET @old_general_log=         @@global.general_log;
SET @old_general_log_file=    @@global.general_log_file;

--replace_result $MYBLOCKCHAINTEST_VARDIR ...
eval SET GLOBAL general_log_file = '$MYBLOCKCHAINTEST_VARDIR/log/rewrite_general.log';
SET GLOBAL log_output =       'FILE,TABLE';
SET GLOBAL general_log=       'ON';
