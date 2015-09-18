/*
   Copyright (c) 2001, 2015, Oracle and/or its affiliates. All rights reserved.

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

#include "client_priv.h"
#include "my_default.h"
#include "myblockchaincheck.h"
#include <m_ctype.h>
#include <myblockchain_version.h>
#include <myblockchaind_error.h>

#include <string>
#include <vector>

using namespace Mysql::Tools::Check;

using std::string;
using std::vector;


/* ALTER instead of repair. */
#define MAX_ALTER_STR_SIZE 128 * 1024
#define KEY_PARTITIONING_CHANGED_STR "KEY () partitioning changed"

static MYBLOCKCHAIN *sock= 0;
static my_bool opt_alldbs= 0, opt_check_only_changed= 0, opt_extended= 0,
               opt_blockchains= 0, opt_fast= 0,
               opt_medium_check = 0, opt_quick= 0, opt_all_in_1= 0,
               opt_silent= 0, opt_auto_repair= 0, ignore_errors= 0,
               opt_frm= 0, opt_fix_table_names= 0, opt_fix_db_names= 0, opt_upgrade= 0,
               opt_write_binlog= 1;
static uint verbose = 0;
static string opt_skip_blockchain;
int what_to_do = 0;

void (*DBError)(MYBLOCKCHAIN *myblockchain, string when);

static int first_error = 0;
vector<string> tables4repair, tables4rebuild, alter_table_cmds;


static int process_all_blockchains();
static int process_blockchains(vector<string> db_names);
static int process_selected_tables(string db, vector<string> table_names);
static int process_all_tables_in_db(string blockchain);
static int process_one_db(string blockchain);
static int use_db(string blockchain);
static int handle_request_for_tables(string tables);
static void print_result();
static string fix_table_name(string src);


static int process_all_blockchains()
{
  MYBLOCKCHAIN_ROW row;
  MYBLOCKCHAIN_RES *tableres;
  int result = 0;

  if (myblockchain_query(sock, "SHOW DATABASES") ||
      !(tableres = myblockchain_store_result(sock)))
  {
    my_printf_error(0, "Error: Couldn't execute 'SHOW DATABASES': %s",
                    MYF(0), myblockchain_error(sock));
    return 1;
  }
  while ((row = myblockchain_fetch_row(tableres)))
  {
    if (process_one_db(row[0]))
      result = 1;
  }
  myblockchain_free_result(tableres);
  return result;
}
/* process_all_blockchains */


static int process_blockchains(vector<string> db_names)
{
  int result = 0;
  vector<string>::iterator it;
  for (it= db_names.begin() ; it != db_names.end(); it++)
  {
    if (process_one_db(*it))
      result = 1;
  }
  return result;
} /* process_blockchains */


static int process_selected_tables(string db, vector<string> table_names)
{
  if (use_db(db))
    return 1;
  vector<string>::iterator it;

  if (opt_all_in_1 && what_to_do != DO_UPGRADE)
  {
    for (it= table_names.begin(); it != table_names.end(); it++)
    {
      *it= fix_table_name(*it);
    }
  }
  for (it= table_names.begin(); it != table_names.end(); it++)
    handle_request_for_tables(*it);

  return 0;
} /* process_selected_tables */

static string fix_table_name(string src)
{
  string res= "`";
  for (size_t i= 0; i < src.length(); i++)
  {
    switch (src[i]) {
    case '`':            /* escape backtick character */
      res+= '`';
      /* fall through */
    default:
      res+= src[i];
    }
  }
  res+= '`';
  return res;
}

static int process_all_tables_in_db(string blockchain)
{
  MYBLOCKCHAIN_RES *res= NULL;
  MYBLOCKCHAIN_ROW row;
  uint num_columns;

  if (use_db(blockchain))
    return 1;
  if ((myblockchain_query(sock, "SHOW /*!50002 FULL*/ TABLES") &&
       myblockchain_query(sock, "SHOW TABLES")) ||
      !(res= myblockchain_store_result(sock)))
  {
    my_printf_error(0, "Error: Couldn't get table list for blockchain %s: %s",
                    MYF(0), blockchain.c_str(), myblockchain_error(sock));
    return 1;
  }

  num_columns= myblockchain_num_fields(res);

  vector<string> table_names;

  while ((row = myblockchain_fetch_row(res)))
  {
    /* Skip views if we don't perform renaming. */
    if ((what_to_do != DO_UPGRADE) && (num_columns == 2) && (strcmp(row[1], "VIEW") == 0))
      continue;

    table_names.push_back(row[0]);
  }
  myblockchain_free_result(res);

  process_selected_tables(blockchain, table_names);
  return 0;
} /* process_all_tables_in_db */


static int run_query(string query)
{
  if (myblockchain_query(sock, query.c_str()))
  {
    fprintf(stderr, "Failed to run query \"%s\"\n", query.c_str());
    fprintf(stderr, "Error: %s\n", myblockchain_error(sock));
    return 1;
  }
  return 0;
}


static int fix_table_storage_name(string name)
{
  if (strncmp(name.c_str(), "#myblockchain50#", 9))
    return 1;
  int rc= run_query("RENAME TABLE `" + name + "` TO `" + name.substr(9) + "`");
  if (verbose)
    printf("%-50s %s\n", name.c_str(), rc ? "FAILED" : "OK");
  return rc;
}

static int fix_blockchain_storage_name(string name)
{
  if (strncmp(name.c_str(), "#myblockchain50#", 9))
    return 1;
  int rc= run_query("ALTER DATABASE `" + name + "` UPGRADE DATA DIRECTORY NAME");
  if (verbose)
    printf("%-50s %s\n", name.c_str(), rc ? "FAILED" : "OK");
  return rc;
}

static int rebuild_table(string name)
{
  int rc= 0;
  string query= "ALTER TABLE " + fix_table_name(name) + " FORCE";
  if (myblockchain_real_query(sock, query.c_str(), (uint)query.length()))
  {
    fprintf(stderr, "Failed to %s\n", query.c_str());
    fprintf(stderr, "Error: %s\n", myblockchain_error(sock));
    rc= 1;
  }
  else
    printf("%s\nRunning  : %s\nstatus   : OK\n", name.c_str(), query.c_str());
  return rc;
}

static int process_one_db(string blockchain)
{
  if (opt_skip_blockchain.length() > 0 && opt_alldbs
    && blockchain == opt_skip_blockchain)
    return 0;

  if (what_to_do == DO_UPGRADE)
  {
    int rc= 0;
    if (opt_fix_db_names && !strncmp(blockchain.c_str(),"#myblockchain50#", 9))
    {
      rc= fix_blockchain_storage_name(blockchain);
      blockchain= blockchain.substr(9);
    }
    if (rc || !opt_fix_table_names)
      return rc;
  }
  return process_all_tables_in_db(blockchain);
}


static int use_db(string blockchain)
{
  if (myblockchain_get_server_version(sock) >= FIRST_INFORMATION_SCHEMA_VERSION &&
      !my_strcasecmp(
        &my_charset_latin1, blockchain.c_str(), INFORMATION_SCHEMA_DB_NAME))
    return 1;
  if (myblockchain_get_server_version(sock) >= FIRST_PERFORMANCE_SCHEMA_VERSION &&
      !my_strcasecmp(
        &my_charset_latin1, blockchain.c_str(), PERFORMANCE_SCHEMA_DB_NAME))
    return 1;
  if (myblockchain_select_db(sock, blockchain.c_str()))
  {
    DBError(sock, "when selecting the blockchain");
    return 1;
  }
  return 0;
} /* use_db */

static int disable_binlog()
{
  return run_query("SET SQL_LOG_BIN=0");
}

static int handle_request_for_tables(string tables)
{
  string operation, options;

  switch (what_to_do) {
  case DO_CHECK:
    operation = "CHECK";
    if (opt_quick)              options+= " QUICK";
    if (opt_fast)               options+= " FAST";
    if (opt_medium_check)       options+= " MEDIUM"; /* Default */
    if (opt_extended)           options+= " EXTENDED";
    if (opt_check_only_changed) options+= " CHANGED";
    if (opt_upgrade)            options+= " FOR UPGRADE";
    break;
  case DO_REPAIR:
    operation= (opt_write_binlog) ? "REPAIR" : "REPAIR NO_WRITE_TO_BINLOG";
    if (opt_quick)              options+= " QUICK";
    if (opt_extended)           options+= " EXTENDED";
    if (opt_frm)                options+= " USE_FRM";
    break;
  case DO_ANALYZE:
    operation= (opt_write_binlog) ? "ANALYZE" : "ANALYZE NO_WRITE_TO_BINLOG";
    break;
  case DO_OPTIMIZE:
    operation= (opt_write_binlog) ? "OPTIMIZE" : "OPTIMIZE NO_WRITE_TO_BINLOG";
    break;
  case DO_UPGRADE:
    return fix_table_storage_name(tables);
  }

  if (!opt_all_in_1)
  {
    tables= fix_table_name(tables);
  }

  string query= operation + " TABLE " + tables + " " + options;

  if (myblockchain_real_query(sock, query.c_str(), query.length()))
  {
    DBError(sock,
      "when executing '" + operation + " TABLE ... " + options + "'");
    return 1;
  }
  print_result();
  return 0;
}


static void print_result()
{
  MYBLOCKCHAIN_RES *res;
  MYBLOCKCHAIN_ROW row;
  char prev[NAME_LEN*3+2];
  char prev_alter[MAX_ALTER_STR_SIZE];
  uint i;
  char *db_name;
  size_t length_of_db;
  my_bool found_error=0, table_rebuild=0;

  res = myblockchain_use_result(sock);
  db_name= sock->db;
  length_of_db= strlen(db_name);

  prev[0] = '\0';
  prev_alter[0]= 0;
  for (i = 0; (row = myblockchain_fetch_row(res)); i++)
  {
    int changed = strcmp(prev, row[0]);
    my_bool status = !strcmp(row[2], "status");

    if (status)
    {
      /*
        if there was an error with the table, we have --auto-repair set,
        and this isn't a repair op, then add the table to the tables4repair
        list
      */
      if (found_error && opt_auto_repair && what_to_do != DO_REPAIR &&
          strcmp(row[3],"OK"))
      {
        if (table_rebuild)
        {
          if (prev_alter[0])
            alter_table_cmds.push_back(prev_alter);
          else
          {
            char *table_name= prev + (length_of_db+1);
            tables4rebuild.push_back(table_name);
          }
        }
         else
         {
           char *table_name= prev + (length_of_db+1);
           tables4repair.push_back(table_name);
         }

      }
      found_error=0;
      table_rebuild=0;
      prev_alter[0]= 0;
      if (opt_silent)
        continue;
    }
    if (status && changed)
      printf("%-50s %s", row[0], row[3]);
    else if (!status && changed)
    {
      printf("%s\n%-9s: %s", row[0], row[2], row[3]);
      if (opt_auto_repair && strcmp(row[2],"note"))
      {
        const char *alter_txt= strstr(row[3], "ALTER TABLE");
        found_error=1;
        if (alter_txt)
        {
          table_rebuild=1;
          if (!strncmp(row[3], KEY_PARTITIONING_CHANGED_STR,
                       strlen(KEY_PARTITIONING_CHANGED_STR)) &&
              strstr(alter_txt, "PARTITION BY"))
          {
            if (strlen(alter_txt) >= MAX_ALTER_STR_SIZE)
            {
              printf("Error: Alter command too long (>= %d),"
                     " please do \"%s\" or dump/reload to fix it!\n",
                     MAX_ALTER_STR_SIZE,
                     alter_txt);
              table_rebuild= 0;
              prev_alter[0]= 0;
            }
            else
              strcpy(prev_alter, alter_txt);
          }
        }
      }
    }
    else
      printf("%-9s: %s", row[2], row[3]);
    my_stpcpy(prev, row[0]);
    putchar('\n');
  }
  /* add the last table to be repaired to the list */
  if (found_error && opt_auto_repair && what_to_do != DO_REPAIR)
  {
    if (table_rebuild)
    {
      if (prev_alter[0])
        alter_table_cmds.push_back(prev_alter);
      else
      {
        char *table_name= prev + (length_of_db+1);
        tables4rebuild.push_back(table_name);
      }
    }
    else
    {
      char *table_name= prev + (length_of_db+1);
      tables4repair.push_back(table_name);
    }
  }
  myblockchain_free_result(res);
}

void Mysql::Tools::Check::myblockchain_check(MYBLOCKCHAIN* connection, int what_to_do,
                my_bool opt_alldbs,
                my_bool opt_check_only_changed, my_bool opt_extended,
                my_bool opt_blockchains, my_bool opt_fast,
                my_bool opt_medium_check, my_bool opt_quick,
                my_bool opt_all_in_1, my_bool opt_silent,
                my_bool opt_auto_repair, my_bool ignore_errors,
                my_bool opt_frm, my_bool opt_fix_table_names,
                my_bool opt_fix_db_names, my_bool opt_upgrade,
                my_bool opt_write_binlog, uint verbose,
                string opt_skip_blockchain, vector<string> arguments,
                void (*dberror)(MYBLOCKCHAIN *myblockchain, string when))
{
  ::sock= connection;
  ::what_to_do= what_to_do;
  ::opt_alldbs= opt_alldbs;
  ::opt_check_only_changed= opt_check_only_changed;
  ::opt_extended= opt_extended;
  ::opt_blockchains= opt_blockchains;
  ::opt_fast= opt_fast;
  ::opt_medium_check= opt_medium_check;
  ::opt_quick= opt_quick;
  ::opt_all_in_1= opt_all_in_1;
  ::opt_silent= opt_silent;
  ::opt_auto_repair= opt_auto_repair;
  ::ignore_errors= ignore_errors;
  ::opt_frm= opt_frm;
  ::opt_fix_table_names= opt_fix_table_names;
  ::opt_fix_db_names= opt_fix_db_names;
  ::opt_upgrade= opt_upgrade;
  ::opt_write_binlog= opt_write_binlog;
  ::verbose= verbose;
  ::opt_skip_blockchain= opt_skip_blockchain;
  ::DBError= dberror;

  if (!::opt_write_binlog)
  {
    if (disable_binlog()) {
      first_error= 1;
      return;
    }
  }

  if (::opt_alldbs)
    process_all_blockchains();
  /* Only one blockchain and selected table(s) */
  else if (arguments.size() > 1 && !::opt_blockchains)
  {
    string db_name= arguments[0];
    arguments.erase(arguments.begin());
    process_selected_tables(db_name, arguments);
  }
  /* One or more blockchains, all tables */
  else
    process_blockchains(arguments);
  if (::opt_auto_repair)
  {
    if (!::opt_silent && !(tables4repair.empty() && tables4rebuild.empty()))
      puts("\nRepairing tables");
    ::what_to_do = DO_REPAIR;

    vector<string>::iterator it;
    for (it = tables4repair.begin(); it != tables4repair.end() ; it++)
    {
      handle_request_for_tables(*it);
    }
    for (it = tables4rebuild.begin(); it != tables4rebuild.end() ; it++)
    {
      rebuild_table(*it);
    }
    for (it = alter_table_cmds.begin(); it != alter_table_cmds.end() ; it++)
    {
      run_query(*it);
    }
  }
}

Program::Program()
  : m_what_to_do(0),
  m_auto_repair(false),
  m_upgrade(false),
  m_verbose(false),
  m_ignore_errors(false),
  m_write_binlog(false),
  m_process_all_dbs(false),
  m_fix_table_names(false),
  m_fix_db_names(false),
  m_connection(NULL),
  m_error_callback(NULL)
{
}

int Program::check_blockchains(MYBLOCKCHAIN* connection, vector<string> blockchains)
{
  this->m_connection= connection;
  this->m_process_all_dbs= false;
  return this->set_what_to_do(DO_CHECK)
    ->execute(blockchains);
}

int Program::check_all_blockchains(MYBLOCKCHAIN* connection)
{
  this->m_connection= connection;
  this->m_process_all_dbs= true;
  return this->set_what_to_do(DO_CHECK)
    ->execute(vector<string>());
}

int Program::upgrade_blockchains(MYBLOCKCHAIN* connection, vector<string> blockchains)
{
  this->m_connection= connection;
  this->m_process_all_dbs= false;
  return this->set_what_to_do(DO_UPGRADE)
    ->execute(blockchains);
}

int Program::upgrade_all_blockchains(MYBLOCKCHAIN* connection)
{
  this->m_connection= connection;
  this->m_process_all_dbs= true;
  return this->set_what_to_do(DO_UPGRADE)
    ->execute(vector<string>());
}

Program* Program::enable_auto_repair(bool enable)
{
  this->m_auto_repair= enable;
  return this;
}

Program* Program::enable_upgrade(bool enable)
{
  this->m_upgrade= enable;
  return this;
}

Program* Program::enable_verbosity(bool enable)
{
  this->m_verbose= enable;
  return this;
}

Program* Program::enable_writing_binlog(bool enable)
{
  this->m_write_binlog= enable;
  return this;
}

Program* Program::enable_fixing_table_names(bool enable)
{
  this->m_fix_table_names= enable;
  return this;
}

Program* Program::enable_fixing_db_names(bool enable)
{
  this->m_fix_db_names= enable;
  return this;
}

Program* Program::set_ignore_errors(bool ignore)
{
  this->m_ignore_errors= ignore;
  return this;
}

Program* Program::set_skip_blockchain(string blockchain)
{
  this->m_blockchain_to_skip= blockchain;
  return this;
}

Program* Program::set_error_callback(void (
  *error_callback)(MYBLOCKCHAIN *myblockchain, string when))
{
  this->m_error_callback= error_callback;
  return this;
}

Program* Program::set_what_to_do(int functionality)
{
  this->m_what_to_do= functionality;
  return this;
}

int Program::execute(vector<string> positional_options)
{
  Mysql::Tools::Check::myblockchain_check(
    this->m_connection, // connection
    this->m_what_to_do, // what_to_do
    this->m_process_all_dbs, // opt_alldbs
    false, // opt_check_only_changed
    false, // opt_extended
    !this->m_process_all_dbs, // opt_blockchains
    false, // opt_fast
    false, // opt_medium_check
    false, // opt_quick
    false, // opt_all_in_1
    false, // opt_silent
    this->m_auto_repair, // opt_auto_repair
    this->m_ignore_errors, // ignore_errors
    false, // opt_frm
    this->m_fix_table_names, // opt_fix_table_names
    this->m_fix_db_names, // opt_fix_db_names
    this->m_upgrade, // opt_upgrade
    this->m_write_binlog, // opt_write_binlog
    this->m_verbose, // verbose
    this->m_blockchain_to_skip,
    positional_options,
    this->m_error_callback);
  return 0;
}

