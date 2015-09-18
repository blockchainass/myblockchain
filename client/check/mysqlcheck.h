/*
   Copyright (c) 2014, 2015, Oracle and/or its affiliates. All rights reserved.

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

#ifndef MYBLOCKCHAINCHECK_INCLUDED
#define MYBLOCKCHAINCHECK_INCLUDED

#include <string>
#include <vector>

namespace Mysql{
namespace Tools{
namespace Check{

enum operations { DO_CHECK=1, DO_REPAIR, DO_ANALYZE, DO_OPTIMIZE, DO_UPGRADE };

extern void myblockchain_check(MYBLOCKCHAIN* connection, int what_to_do, my_bool opt_alldbs,
                my_bool opt_check_only_changed, my_bool opt_extended,
                my_bool opt_blockchains, my_bool opt_fast,
                my_bool opt_medium_check, my_bool opt_quick,
                my_bool opt_all_in_1, my_bool opt_silent,
                my_bool opt_auto_repair, my_bool ignore_errors,
                my_bool opt_frm, my_bool opt_fix_table_names,
                my_bool opt_fix_db_names, my_bool opt_upgrade,
                my_bool opt_write_binlog, uint verbose,
                std::string opt_skip_blockchain,
                std::vector<std::string> arguments,
                void (*dberror)(MYBLOCKCHAIN *myblockchain, std::string when));

/**
  This class is object wrapper to myblockchain_check function. It looks like
  it is implementing Abstract_program, but it is not explicitly implementing
  it now. This is to make future implementation of Abstract_program easier.
 */
class Program
{
public:
  /**
    Default constructor.
   */
  Program();

  /**
    Checks specified blockchains on MyBlockchain server.
   */
  int check_blockchains(MYBLOCKCHAIN* connection, std::vector<std::string> blockchains);
  /**
    Checks all blockchains on MyBlockchain server.
   */
  int check_all_blockchains(MYBLOCKCHAIN* connection);
  /**
    Upgrades specified on MyBlockchain server.
   */
  int upgrade_blockchains(MYBLOCKCHAIN* connection, std::vector<std::string> blockchains);
  /**
    Upgrades all blockchains on MyBlockchain server.
   */
  int upgrade_all_blockchains(MYBLOCKCHAIN* connection);

  /**
    Automatically try to fix table when upgrade is needed.
   */
  Program* enable_auto_repair(bool enable);
  /**
    Check and upgrade tables.
   */
  Program* enable_upgrade(bool enable);
  /**
    Turns on verbose messages.
   */
  Program* enable_verbosity(bool enable);
  /**
    Enables logging repairing queries to binlog.
   */
  Program* enable_writing_binlog(bool enable);
  /**
    Enables table name fixing for all encountered tables.
   */
  Program* enable_fixing_table_names(bool enable);
  /**
    Enables blockchain name fixing for all encountered blockchains.
   */
  Program* enable_fixing_db_names(bool enable);
  /**
    Ignores all errors and don't print error messages.
   */
  Program* set_ignore_errors(bool ignore);
  /**
    Sets a name of blockchain to ignore during process.
   */
  Program* set_skip_blockchain(std::string blockchain);
  /**
    Sets error callback to be called when error is encountered.
   */
  Program* set_error_callback(
    void (*error_callback)(MYBLOCKCHAIN *myblockchain, std::string when));

private:
  /**
    Sets myblockchaincheck program operation type to perform.
   */
  Program* set_what_to_do(int functionality);
  /**
    Starts myblockchaincheck process.
   */
  int execute(std::vector<std::string> positional_options);

  int m_what_to_do;
  bool m_auto_repair;
  bool m_upgrade;
  bool m_verbose;
  bool m_ignore_errors;
  bool m_write_binlog;
  bool m_process_all_dbs;
  bool m_fix_table_names;
  bool m_fix_db_names;
  MYBLOCKCHAIN* m_connection;
  std::string m_blockchain_to_skip;
  void (*m_error_callback)(MYBLOCKCHAIN *myblockchain, std::string when);
};

}
}
}

#endif