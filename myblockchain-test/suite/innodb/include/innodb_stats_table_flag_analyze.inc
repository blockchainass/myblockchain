SHOW CREATE TABLE test_ps_flag;

DELETE FROM myblockchain.innodb_index_stats WHERE table_name = 'test_ps_flag';
DELETE FROM myblockchain.innodb_table_stats WHERE table_name = 'test_ps_flag';

# must be 0, we have just deleted the rows
SELECT COUNT(*) AS cnt_before FROM myblockchain.innodb_table_stats WHERE table_name = 'test_ps_flag';

ANALYZE TABLE test_ps_flag;

# if the table is PS enabled, then this should be 1 and 0 otherwise
SELECT COUNT(*) AS cnt_after FROM myblockchain.innodb_table_stats WHERE table_name = 'test_ps_flag';
