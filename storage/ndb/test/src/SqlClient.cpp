/*
   Copyright (C) 2008 MyBlockchain AB
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

#include <SqlClient.hpp>
#include <NDBT_Output.hpp>
#include <NdbSleep.h>

SqlClient::SqlClient(const char* _user,
                       const char* _password,
                       const char* _group_suffix):
  connected(false),
  myblockchain(NULL),
  free_myblockchain(false)
{

  const char* env= getenv("MYBLOCKCHAIN_HOME");
  if (env && strlen(env))
  {
    default_file.assfmt("%s/my.cnf", env);
  }

  if (_group_suffix != NULL){
    default_group.assfmt("client%s", _group_suffix);
  }
  else {
    default_group.assign("client.1.atrt");
  }

  g_info << "default_file: " << default_file.c_str() << endl;
  g_info << "default_group: " << default_group.c_str() << endl;

  user.assign(_user);
  password.assign(_password);
}


SqlClient::SqlClient(MYBLOCKCHAIN* myblockchain):
  connected(true),
  myblockchain(myblockchain),
  free_myblockchain(false)
{
}


SqlClient::~SqlClient(){
  disconnect();
}


bool
SqlClient::isConnected(){
  if (connected == true)
  {
    require(myblockchain);
    return true;
  }
  return connect() == 0;
}


int
SqlClient::connect(){
  disconnect();

//  myblockchain_debug("d:t:O,/tmp/client.trace");

  if ((myblockchain= myblockchain_init(NULL)) == NULL){
    g_err << "myblockchain_init failed" << endl;
    return -1;
  }

  /* Load connection parameters file and group */
  if (myblockchain_options(myblockchain, MYBLOCKCHAIN_READ_DEFAULT_FILE, default_file.c_str()) ||
      myblockchain_options(myblockchain, MYBLOCKCHAIN_READ_DEFAULT_GROUP, default_group.c_str()))
  {
    g_err << "myblockchain_options failed" << endl;
    disconnect();
    return 1;
  }

  /*
    Connect, read settings from my.cnf
    NOTE! user and password can be stored there as well
   */
  if (myblockchain_real_connect(myblockchain, NULL, user.c_str(),
                         password.c_str(), "atrt", 0, NULL, 0) == NULL)
  {
    g_err  << "Connection to atrt server failed: "<< myblockchain_error(myblockchain) << endl;
    disconnect();
    return -1;
  }

  g_err << "Connected to MyBlockchain " << myblockchain_get_server_info(myblockchain)<< endl;

  connected = true;
  return 0;
}


bool
SqlClient::waitConnected(int timeout) {
  timeout*= 10;
  while(!isConnected()){
    if (timeout-- == 0)
      return false;
    NdbSleep_MilliSleep(100);
  }
  return true;
}


void
SqlClient::disconnect(){
  if (myblockchain != NULL){
    if (free_myblockchain)
      myblockchain_close(myblockchain);
    myblockchain= NULL;
  }
  connected = false;
}


static bool is_int_type(enum_field_types type){
  switch(type){
  case MYBLOCKCHAIN_TYPE_TINY:
  case MYBLOCKCHAIN_TYPE_SHORT:
  case MYBLOCKCHAIN_TYPE_LONGLONG:
  case MYBLOCKCHAIN_TYPE_INT24:
  case MYBLOCKCHAIN_TYPE_LONG:
  case MYBLOCKCHAIN_TYPE_ENUM:
    return true;
  default:
    return false;
  }
  return false;
}


bool
SqlClient::runQuery(const char* sql,
                    const Properties& args,
                    SqlResultSet& rows){

  rows.clear();
  if (!isConnected())
    return false;

  g_debug << "runQuery: " << endl
          << " sql: '" << sql << "'" << endl;


  MYBLOCKCHAIN_STMT *stmt= myblockchain_stmt_init(myblockchain);
  if (myblockchain_stmt_prepare(stmt, sql, strlen(sql)))
  {
    g_err << "Failed to prepare: " << myblockchain_error(myblockchain) << endl;
    return false;
  }

  uint params= myblockchain_stmt_param_count(stmt);
  MYBLOCKCHAIN_BIND bind_param[params];
  bzero(bind_param, sizeof(bind_param));

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
      bind_param[i].buffer_length= strlen(val_s);
      g_debug << " param" << name.c_str() << ": " << val_s << endl;
      break;
    default:
      require(false);
      break;
    }
  }
  if (myblockchain_stmt_bind_param(stmt, bind_param))
  {
    g_err << "Failed to bind param: " << myblockchain_error(myblockchain) << endl;
    myblockchain_stmt_close(stmt);
    return false;
  }

  if (myblockchain_stmt_execute(stmt))
  {
    g_err << "Failed to execute: " << myblockchain_error(myblockchain) << endl;
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
    g_err << "Failed to store result: " << myblockchain_error(myblockchain) << endl;
    myblockchain_stmt_close(stmt);
    return false;
  }

  uint row= 0;
  MYBLOCKCHAIN_RES* res= myblockchain_stmt_result_metadata(stmt);
  if (res != NULL)
  {
    MYBLOCKCHAIN_FIELD *fields= myblockchain_fetch_fields(res);
    uint num_fields= myblockchain_num_fields(res);
    MYBLOCKCHAIN_BIND bind_result[num_fields];
    bzero(bind_result, sizeof(bind_result));

    for (uint i= 0; i < num_fields; i++)
    {
      if (is_int_type(fields[i].type)){
        bind_result[i].buffer_type= MYBLOCKCHAIN_TYPE_LONG;
        bind_result[i].buffer= malloc(sizeof(int));
      }
      else
      {
        uint max_length= fields[i].max_length + 1;
        bind_result[i].buffer_type= MYBLOCKCHAIN_TYPE_STRING;
        bind_result[i].buffer= malloc(max_length);
        bind_result[i].buffer_length= max_length;
      }
    }

    if (myblockchain_stmt_bind_result(stmt, bind_result)){
      g_err << "Failed to bind result: " << myblockchain_error(myblockchain) << endl;
      myblockchain_stmt_close(stmt);
      return false;
    }

    while (myblockchain_stmt_fetch(stmt) != MYBLOCKCHAIN_NO_DATA)
    {
      Properties curr(true);
      for (uint i= 0; i < num_fields; i++){
        if (is_int_type(fields[i].type))
          curr.put(fields[i].name, *(int*)bind_result[i].buffer);
        else
          curr.put(fields[i].name, (char*)bind_result[i].buffer);
      }
      rows.put("row", row++, &curr);
    }

    myblockchain_free_result(res);

    for (uint i= 0; i < num_fields; i++)
      free(bind_result[i].buffer);

  }

  // Save stats in result set
  rows.put("rows", row);
  rows.put("affected_rows", myblockchain_affected_rows(myblockchain));
  rows.put("myblockchain_errno", myblockchain_errno(myblockchain));
  rows.put("myblockchain_error", myblockchain_error(myblockchain));
  rows.put("myblockchain_sqlstate", myblockchain_sqlstate(myblockchain));
  rows.put("insert_id", myblockchain_insert_id(myblockchain));

  myblockchain_stmt_close(stmt);
  return true;
}


bool
SqlClient::doQuery(const char* query){
  const Properties args;
  SqlResultSet result;
  return doQuery(query, args, result);
}


bool
SqlClient::doQuery(const char* query, SqlResultSet& result){
  Properties args;
  return doQuery(query, args, result);
}


bool
SqlClient::doQuery(const char* query, const Properties& args,
                   SqlResultSet& result){
  if (!runQuery(query, args, result))
    return false;
  result.get_row(0); // Load first row
  return true;
}


bool
SqlClient::doQuery(BaseString& str){
  return doQuery(str.c_str());
}


bool
SqlClient::doQuery(BaseString& str, SqlResultSet& result){
  return doQuery(str.c_str(), result);
}


bool
SqlClient::doQuery(BaseString& str, const Properties& args,
                   SqlResultSet& result){
  return doQuery(str.c_str(), args, result);
}




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

uint SqlResultSet::insertId(){
  return get_int("insert_id");
}

uint SqlResultSet::affectedRows(){
  return get_int("affected_rows");
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
  require(get(name, &value));
  return value;
}

const char* SqlResultSet::get_string(const char* name){
  const char* value;
  require(get(name, &value));
  return value;
}
