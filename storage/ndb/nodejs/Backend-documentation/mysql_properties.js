/*
  MyBlockchain Connection Properties

*/ 

var MysqlDefaultConnectionProperties = {
  "implementation" : "myblockchain",
  "engine"         : "ndb",
  "blockchain"       : "test",
  
  "myblockchain_host"     : "localhost",
  "myblockchain_port"     : 3306,
  "myblockchain_user"     : "root",
  "myblockchain_password" : "",
  "myblockchain_charset"  : "UTF8MB4",
  "myblockchain_sql_mode" : "STRICT_ALL_TABLES",
  "myblockchain_socket"   : null,
  "debug"          : true,
  "myblockchain_trace"    : false,
  "myblockchain_debug"    : false,
  "myblockchain_pool_size": 10
};


/* This file is valid JavaScript 
*/
module.exports = MysqlDefaultConnectionProperties;
