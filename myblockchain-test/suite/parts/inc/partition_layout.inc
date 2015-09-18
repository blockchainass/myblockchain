# inc/partition_layout.inc
#
# Print partitioning related informations about the table t1
#

eval SHOW CREATE TABLE t1;

# Optional (most probably issues with separators and case sensitivity)
# listing of files belonging to the table t1
if ($ls)
{
   let $MYBLOCKCHAIND_DATADIR= `select @@datadir`;
   --replace_result $MYBLOCKCHAIND_DATADIR MYBLOCKCHAIND_DATADIR #p# #P# #sp# #SP#
   --list_files $MYBLOCKCHAIND_DATADIR/test t1*
}
