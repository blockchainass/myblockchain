--disable_result_log
write_file $MYBLOCKCHAINTEST_VARDIR/tmp/explain.json;
{
 "table": 
 {
   "id": 1,
   "select_type": "SIMPLE",
   "table_name": "t1",
   "join_type": "index",
   "key": "i1",
   "key_length": "5",
   "rows": 3,
   "extra": 
   [
     "Using index"
   ]
 }
}
EOF
--require r/python_with_json.require
--replace_regex  /[-]*// /FILE.[\/\\:_\.0-9A-Za-z]*/Validation:/
--exec python $MYBLOCKCHAIN_TEST_DIR/suite/opt_trace/validate_json.py $MYBLOCKCHAINTEST_VARDIR/tmp/explain.json
--remove_file '$MYBLOCKCHAINTEST_VARDIR/tmp/explain.json'
--enable_result_log

