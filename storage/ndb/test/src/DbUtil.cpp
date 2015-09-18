/*
   Copyright (C) 2007, 2008 MyBlockchain AB, 2008, 2009 Sun Microsystems, Inc.
    All rights reserved. Use is subject to license terms.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

/* DbUtil.cpp: implementation of the blockchain utilities class.*/

#include "DbUtil.hpp"
#include <NdbSleep.h>
#include <NdbAutoPtr.hpp>

/* Constructors */

DbUtil::DbUtil(const char* _dbname,
               const char* _suffix):
  m_myblockchain(NULL),
  m_free_myblockchain(true),
  m_connected(false),
  m_user("root"),
  m_pass(""),
  m_dbname(_dbname),
  m_silent(false)
{
  const char* env= getenv("MYBLOCKCHAIN_HOME");
  if (env && strlen(env))
  {
    m_default_file.assfmt("%s/my.cnf", env);
  }

  if (_suffix != NULL){
    m_default_group.assfmt("client%s", _suffix);
  }
  else {
    m_default_group.assign("client.1.master");
  }

  ndbout << "default_file: " << m_default_file.c_str() << endl;
  ndbout << "default_group: " << m_default_group.c_str() << endl;
}



DbUtil::DbUtil(MYBLOCKCHAIN* myblockchain):
  m_myblockchain(myblockchain),
  m_free_myblockchain(false),
  m_connected(true)
{
}


bool
DbUtil::isConnected(){
  if (m_connected == true)
  {
    require(m_myblockchain);
    return true;
  }
  return connect();
}


bool
DbUtil::waitConnected(int timeout) {
  timeout*= 10;
  while(!isConnected()){
    if (timeout-- == 0)
      return false;
    NdbSleep_MilliSleep(100);
  }
  return true;
}


void
DbUtil::disconnect(){
  if (m_myblockchain != NULL){
    if (m_free_myblockchain)
      myblockchain_close(m_myblockchain);
    m_myblockchain= NULL;
  }
  m_connected = false;
}


/* Destructor */

DbUtil::~DbUtil()
{
  disconnect();
}

/* Database Login */

bool
DbUtil::blockchainLogin(const char* system, const char* usr,
                           const char* password, unsigned int portIn,
                           const char* sockIn, bool transactional)
{
  if (!(m_myblockchain = myblockchain_init(NULL)))
  {
    myerror("DB Login-> myblockchain_init() failed");
    return false;
  }
  setUser(usr);
  setHost(system);
  setPassword(password);
  setPort(portIn);
  setSocket(sockIn);
  m_dbname.assign("test");

  if (!(myblockchain_real_connect(m_myblockchain, 
                           m_host.c_str(), 
                           m_user.c_str(), 
                           m_pass.c_str(), 
                           m_dbname.c_str(),
                           m_port, 
                           m_socket.c_str(), 0)))
  {
    myerror("connection failed");
    disconnect();
    return false;
  }

  m_myblockchain->reconnect = TRUE;

  /* set AUTOCOMMIT */
  if(!transactional)
    myblockchain_autocommit(m_myblockchain, TRUE);
  else
    myblockchain_autocommit(m_myblockchain, FALSE);

  #ifdef DEBUG
    printf("\n\tConnected to MyBlockchain server version: %s (%lu)\n\n", 
           myblockchain_get_server_info(m_myblockchain),
           (unsigned long) myblockchain_get_server_version(m_myblockchain));
  #endif
  selectDb();
  m_connected= true;
  return true;
}

/* Database Connect */

bool
DbUtil::connect()
{
  if (!(m_myblockchain = myblockchain_init(NULL)))
  {
    myerror("DB connect-> myblockchain_init() failed");
    return false;
  }

  /* Load connection parameters file and group */
  if (myblockchain_options(m_myblockchain, MYBLOCKCHAIN_READ_DEFAULT_FILE, m_default_file.c_str()) ||
      myblockchain_options(m_myblockchain, MYBLOCKCHAIN_READ_DEFAULT_GROUP, m_default_group.c_str()))
  {
    myerror("DB Connect -> myblockchain_options failed");
    disconnect();
    return false;
  }

  /*
    Connect, read settings from my.cnf
    NOTE! user and password can be stored there as well
  */
  if (myblockchain_real_connect(m_myblockchain, NULL,
                         m_user.c_str(),
                         m_pass.c_str(),
                         m_dbname.c_str(),
                         0, NULL, 0) == NULL)
  {
    myerror("connection failed");
    disconnect();
    return false;
  }
  selectDb();
  m_connected= true;
  require(m_myblockchain);
  return true;
}


/* Database Logout */

void
DbUtil::blockchainLogout()
{
  if (m_myblockchain){
    #ifdef DEBUG
      printf("\n\tClosing the MyBlockchain blockchain connection ...\n\n");
    #endif
    myblockchain_close(m_myblockchain);
  }
}

/* Prepare MyBlockchain Statements Cont */

MYBLOCKCHAIN_STMT *STDCALL 
DbUtil::myblockchainSimplePrepare(const char *query)
{
  #ifdef DEBUG
    printf("Inside DbUtil::myblockchainSimplePrepare\n");
  #endif
  MYBLOCKCHAIN_STMT *my_stmt= myblockchain_stmt_init(this->getMysql());
  if (my_stmt && myblockchain_stmt_prepare(my_stmt, query, (unsigned long)strlen(query))){
    this->printStError(my_stmt,"Prepare Statement Failed");
    myblockchain_stmt_close(my_stmt);
    return NULL;
  }
  return my_stmt;
}

/* Close MyBlockchain Statements Handle */

void 
DbUtil::myblockchainCloseStmHandle(MYBLOCKCHAIN_STMT *my_stmt)
{
  myblockchain_stmt_close(my_stmt);
}
 
/* Error Printing */

void
DbUtil::printError(const char *msg)
{
  if (m_myblockchain && myblockchain_errno(m_myblockchain))
  {
    if (m_myblockchain->server_version)
      printf("\n [MyBlockchain-%s]", m_myblockchain->server_version);
    else
      printf("\n [MyBlockchain]");
      printf("[%d] %s\n", getErrorNumber(), getError());
  }
  else if (msg)
    printf(" [MyBlockchain] %s\n", msg);
}

void
DbUtil::printStError(MYBLOCKCHAIN_STMT *stmt, const char *msg)
{
  if (stmt && myblockchain_stmt_errno(stmt))
  {
    if (m_myblockchain && m_myblockchain->server_version)
      printf("\n [MyBlockchain-%s]", m_myblockchain->server_version);
    else
      printf("\n [MyBlockchain]");

    printf("[%d] %s\n", myblockchain_stmt_errno(stmt),
    myblockchain_stmt_error(stmt));
  }
  else if (msg)
    printf("[MyBlockchain] %s\n", msg);
}

/* Select which blockchain to use */

bool
DbUtil::selectDb()
{
  if ((getDbName()) != NULL)
  {
    if(myblockchain_select_db(m_myblockchain, this->getDbName()))
    {
      //printError("myblockchain_select_db failed");
      return false;
    }
    return true;   
  }
  printError("getDbName() == NULL");
  return false;
}

bool
DbUtil::selectDb(const char * m_db)
{
  {
    if(myblockchain_select_db(m_myblockchain, m_db))
    {
      printError("myblockchain_select_db failed");
      return false;
    }
    return true;
  }
}

bool
DbUtil::createDb(BaseString& m_db)
{
  BaseString stm;
  setDbName(m_db.c_str());
  
  {
    if(selectDb())
    {
      stm.assfmt("DROP DATABASE %s", m_db.c_str());
      if(!doQuery(m_db.c_str()))
        return false;
    }
    stm.assfmt("CREATE DATABASE %s", m_db.c_str());
    if(!doQuery(m_db.c_str()))
      return false;
    return true;
  }
}


/* Count Table Rows */

unsigned long long
DbUtil::selectCountTable(const char * table)
{
  BaseString query;
  SqlResultSet result;

  query.assfmt("select count(*) as count from %s", table);
  if (!doQuery(query, result)) {
    printError("select count(*) failed");
    return -1;
  }
   return result.columnAsLong("count");
}


/* Run Simple Queries */


bool
DbUtil::runQuery(const char* sql,
                 const Properties& args,
                 SqlResultSet& rows){

  clear_error();
  rows.clear();
  if (!isConnected())
    return false;
  require(m_myblockchain);

  g_debug << "runQuery: " << endl
          << " sql: '" << sql << "'" << endl;


  MYBLOCKCHAIN_STMT *stmt= myblockchain_stmt_init(m_myblockchain);
  if (myblockchain_stmt_prepare(stmt, sql, (unsigned long)strlen(sql)))
  {
    report_error("Failed to prepare: ", m_myblockchain);
    return false;
  }

  uint params= myblockchain_stmt_param_count(stmt);
  MYBLOCKCHAIN_BIND *bind_param = new MYBLOCKCHAIN_BIND[params];
  NdbAutoObjArrayPtr<MYBLOCKCHAIN_BIND> _guard(bind_param);

  memset(bind_param, 0, params * sizeof(MYBLOCKCHAIN_BIND));

  for(uint i= 0; i < myblockchain_stmt_param_count(stmt); i++)
  {
    BaseString name;
    name.assfmt("%d", i);
    // Parameters are named 0, 1, 2...
    if (!args.contains(name.c_str()))
    {
      g_err << "param " << i << " missing" << endl;
      require(false);
    }
    PropertiesType t;
    Uint32 val_i;
    const char* val_s;
    args.getTypeOf(name.c_str(), &t);
    switch(t) {
    case PropertiesType_Uint32:
      args.get(name.c_str(), &val_i);
      bind_param[i].buffer_type= MYBLOCKCHAIN_TYPE_LONG;
      bind_param[i].buffer= (char*)&val_i;
      g_debug << " param" << name.c_str() << ": " << val_i << endl;
      break;
    case PropertiesType_char:
      args.get(name.c_str(), &val_s);
      bind_param[i].buffer_type= MYBLOCKCHAIN_TYPE_STRING;
      bind_param[i].buffer= (char*)val_s;
      bind_param[i].buffer_length= (unsigned long)strlen(val_s);
      g_debug << " param" << name.c_str() << ": " << val_s << endl;
      break;
    default:
      require(false);
      break;
    }
  }
  if (myblockchain_stmt_bind_param(stmt, bind_param))
  {
    report_error("Failed to bind param: ", m_myblockchain);
    myblockchain_stmt_close(stmt);
    return false;
  }

  if (myblockchain_stmt_execute(stmt))
  {
    report_error("Failed to execute: ", m_myblockchain);
    myblockchain_stmt_close(stmt);
    return false;
  }

  /*
    Update max_length, making it possible to know how big
    buffers to allocate
  */
  my_bool one= 1;
  myblockchain_stmt_attr_set(stmt, STMT_ATTR_UPDATE_MAX_LENGTH, (void*) &one);

  if (myblockchain_stmt_store_result(stmt))
  {
    report_error("Failed to store result: ", m_myblockchain);
    myblockchain_stmt_close(stmt);
    return false;
  }

  uint row= 0;
  MYBLOCKCHAIN_RES* res= myblockchain_stmt_result_metadata(stmt);
  if (res != NULL)
  {
    MYBLOCKCHAIN_FIELD *fields= myblockchain_fetch_fields(res);
    uint num_fields= myblockchain_num_fields(res);
    MYBLOCKCHAIN_BIND *bind_result = new MYBLOCKCHAIN_BIND[num_fields];
    NdbAutoObjArrayPtr<MYBLOCKCHAIN_BIND> _guard1(bind_result);
    memset(bind_result, 0, num_fields * sizeof(MYBLOCKCHAIN_BIND));

    for (uint i= 0; i < num_fields; i++)
    {
      unsigned long buf_len= sizeof(int);

      switch(fields[i].type){
      case MYBLOCKCHAIN_TYPE_STRING:
        buf_len = fields[i].length + 1;
        break;
      case MYBLOCKCHAIN_TYPE_VARCHAR:
      case MYBLOCKCHAIN_TYPE_VAR_STRING:
        buf_len= fields[i].max_length + 1;
        break;
      case MYBLOCKCHAIN_TYPE_LONGLONG:
        buf_len= sizeof(long long);
        break;
      case MYBLOCKCHAIN_TYPE_LONG:
        buf_len = sizeof(long);
        break;
      default:
        break;
      }
      
      bind_result[i].buffer_type= fields[i].type;
      bind_result[i].buffer= malloc(buf_len);
      bind_result[i].buffer_length= buf_len;
      bind_result[i].is_null = (my_bool*)malloc(sizeof(my_bool));
      * bind_result[i].is_null = 0;
    }

    if (myblockchain_stmt_bind_result(stmt, bind_result)){
      report_error("Failed to bind result: ", m_myblockchain);
      myblockchain_stmt_close(stmt);
      return false;
    }

    while (myblockchain_stmt_fetch(stmt) != MYBLOCKCHAIN_NO_DATA)
    {
      Properties curr(true);
      for (uint i= 0; i < num_fields; i++){
        if (* bind_result[i].is_null)
          continue;
        switch(fields[i].type){
        case MYBLOCKCHAIN_TYPE_STRING:
	  ((char*)bind_result[i].buffer)[fields[i].max_length] = 0;
        case MYBLOCKCHAIN_TYPE_VARCHAR:
        case MYBLOCKCHAIN_TYPE_VAR_STRING:
          curr.put(fields[i].name, (char*)bind_result[i].buffer);
          break;

        case MYBLOCKCHAIN_TYPE_LONGLONG:
          curr.put64(fields[i].name,
                     *(unsigned long long*)bind_result[i].buffer);
          break;

        default:
          curr.put(fields[i].name, *(int*)bind_result[i].buffer);
          break;
        }
      }
      rows.put("row", row++, &curr);
    }

    myblockchain_free_result(res);

    for (uint i= 0; i < num_fields; i++)
    {
      free(bind_result[i].buffer);
      free(bind_result[i].is_null);
    }
  }

  // Save stats in result set
  rows.put("rows", row);
  rows.put64("affected_rows", myblockchain_affected_rows(m_myblockchain));
  rows.put("myblockchain_errno", myblockchain_errno(m_myblockchain));
  rows.put("myblockchain_error", myblockchain_error(m_myblockchain));
  rows.put("myblockchain_sqlstate", myblockchain_sqlstate(m_myblockchain));
  rows.put64("insert_id", myblockchain_insert_id(m_myblockchain));

  myblockchain_stmt_close(stmt);
  return true;
}


bool
DbUtil::doQuery(const char* query){
  const Properties args;
  SqlResultSet result;
  return doQuery(query, args, result);
}


bool
DbUtil::doQuery(const char* query, SqlResultSet& result){
  Properties args;
  return doQuery(query, args, result);
}


bool
DbUtil::doQuery(const char* query, const Properties& args,
                   SqlResultSet& result){
  if (!runQuery(query, args, result))
    return false;
  result.get_row(0); // Load first row
  return true;
}

bool
DbUtil::doQuery(const char* query, const Properties& args){
  SqlResultSet result;
  return doQuery(query, args, result);
}


bool
DbUtil::doQuery(BaseString& str){
  return doQuery(str.c_str());
}


bool
DbUtil::doQuery(BaseString& str, SqlResultSet& result){
  return doQuery(str.c_str(), result);
}


bool
DbUtil::doQuery(BaseString& str, const Properties& args,
                   SqlResultSet& result){
  return doQuery(str.c_str(), args, result);
}


bool
DbUtil::doQuery(BaseString& str, const Properties& args){
  return doQuery(str.c_str(), args);
}


/* Return MyBlockchain Error String */

const char *
DbUtil::getError()
{
  return myblockchain_error(this->getMysql());
}

/* Return MyBlockchain Error Number */

int
DbUtil::getErrorNumber()
{
  return myblockchain_errno(this->getMysql());
}

void
DbUtil::report_error(const char* message, MYBLOCKCHAIN* myblockchain)
{
  m_last_errno= myblockchain_errno(myblockchain);
  m_last_error.assfmt("%d: %s", m_last_errno, myblockchain_error(myblockchain));

  if (!m_silent)
    g_err << message << m_last_error << endl;
}


/* DIE */

void
DbUtil::die(const char *file, int line, const char *expr)
{
  printf("%s:%d: check failed: '%s'\n", file, line, expr);
  abort();
}


/* SqlResultSet */

bool
SqlResultSet::get_row(int row_num){
  if(!get("row", row_num, &m_curr_row)){
    return false;
  }
  return true;
}


bool
SqlResultSet::next(void){
  return get_row(++m_curr_row_num);
}


// Reset iterator
void SqlResultSet::reset(void){
  m_curr_row_num= -1;
  m_curr_row= 0;
}


// Remove row from resultset
void SqlResultSet::remove(){
  BaseString row_name;
  row_name.assfmt("row_%d", m_curr_row_num);
  Properties::remove(row_name.c_str());
}


// Clear all rows and reset iterator
void SqlResultSet::clear(){
  reset();
  Properties::clear();
}


SqlResultSet::SqlResultSet(): m_curr_row(0), m_curr_row_num(-1){
}


SqlResultSet::~SqlResultSet(){
}


const char* SqlResultSet::column(const char* col_name){
  const char* value;
  if (!m_curr_row){
    g_err << "ERROR: SqlResultSet::column("<< col_name << ")" << endl
          << "There is no row loaded, call next() before "
          << "acessing the column values" << endl;
    require(m_curr_row);
  }
  if (!m_curr_row->get(col_name, &value))
    return NULL;
  return value;
}


uint SqlResultSet::columnAsInt(const char* col_name){
  uint value;
  if (!m_curr_row){
    g_err << "ERROR: SqlResultSet::columnAsInt("<< col_name << ")" << endl
          << "There is no row loaded, call next() before "
          << "acessing the column values" << endl;
    require(m_curr_row);
  }
  if (!m_curr_row->get(col_name, &value))
    return (uint)-1;
  return value;
}

unsigned long long SqlResultSet::columnAsLong(const char* col_name){
  unsigned long long value;
  if (!m_curr_row){
    g_err << "ERROR: SqlResultSet::columnAsLong("<< col_name << ")" << endl
          << "There is no row loaded, call next() before "
          << "acessing the column values" << endl;
    require(m_curr_row);
  }
  if (!m_curr_row->get(col_name, &value))
    return (uint)-1;
  return value;
}


unsigned long long SqlResultSet::insertId(){
  return get_long("insert_id");
}


unsigned long long SqlResultSet::affectedRows(){
  return get_long("affected_rows");
}

uint SqlResultSet::numRows(void){
  return get_int("rows");
}


uint SqlResultSet::myblockchainErrno(void){
  return get_int("myblockchain_errno");
}


const char* SqlResultSet::myblockchainError(void){
  return get_string("myblockchain_error");
}


const char* SqlResultSet::myblockchainSqlstate(void){
  return get_string("myblockchain_sqlstate");
}


uint SqlResultSet::get_int(const char* name){
  uint value;
  get(name, &value);
  return value;
}

unsigned long long SqlResultSet::get_long(const char* name){
  unsigned long long value;
  get(name, &value);
  return value;
}


const char* SqlResultSet::get_string(const char* name){
  const char* value;
  get(name, &value);
  return value;
}

/* EOF */

