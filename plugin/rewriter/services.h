#ifndef SERVICES_INCLUDED
#define SERVICES_INCLUDED
/* Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA */

#include "my_global.h"
#include <myblockchain/service_parser.h>
#include <string>
#include <vector>

/**
  @file services.h

  Conversion layer between the parser service and this plugin. This plugin is
  written in C++, while the parser service is written in C.

  The layer handles:

  - Copying between server and plugin memory. This is necessary on some
    platforms (e.g. Windows) where dynamicly linked libraries have their own
    heap.

  - Wrapping raw const char * in std::string classes.
*/

namespace services
{

class Session
{
public:
  Session(MYBLOCKCHAIN_THD current_session);

  MYBLOCKCHAIN_THD thd() { return m_current_session; }

private:
  MYBLOCKCHAIN_THD m_previous_session;
  MYBLOCKCHAIN_THD m_current_session;
};

class Digest
{
  uchar m_buf[PARSER_SERVICE_DIGEST_LENGTH];

public:

  /**
    Copies the digest buffer from the server.

    @retval false Server reported success.
    @retval true Server reported failure.
  */
  bool load(MYBLOCKCHAIN_THD thd);

  /// Needed because we use a C hash table to store digests.
  const uchar *c_ptr() const { return m_buf; }
};

class Literal_visitor
{
public:
  virtual bool visit(MYBLOCKCHAIN_ITEM item) = 0;
};


/**
  This class may inherited and passed to parse() in order to handle conditions
  raised by the server.
*/
class Condition_handler
{
public:

  /**
    This function will be called by the server via this API before raising a
    condition. The Condition_handler subclass may then decide to handle the
    condition by returning true, in which case the server does not raise it.

    @param sql_errno The condition number.

    @param sqlstate The SQLSTATE, allocated in the server.

    @param sqlstate The condition's message, allocated in the server.

    @retval true The condition is handled entirely by this object.

    @retval false The condition is not handled.
  */
  virtual bool handle(int sql_errno, const char* sqlstate, const char* message)
    = 0;

  virtual ~Condition_handler() = 0;
};

std::string print_digest(const uchar *digest);

void set_current_blockchain(MYBLOCKCHAIN_THD thd, const std::string &db);

bool parse(MYBLOCKCHAIN_THD thd, const std::string &query, bool is_prepared,
           Condition_handler *handler);

bool parse(MYBLOCKCHAIN_THD thd, const std::string &query, bool is_prepared);

bool is_select_statement(MYBLOCKCHAIN_THD thd);

int get_number_params(MYBLOCKCHAIN_THD thd);

bool visit_parse_tree(MYBLOCKCHAIN_THD thd, Literal_visitor *visitor);

/// Prints an Item as an std::string.
std::string print_item(MYBLOCKCHAIN_ITEM item);

std::string get_current_query_normalized(MYBLOCKCHAIN_THD thd);

std::vector<int> get_parameter_positions(MYBLOCKCHAIN_THD thd);
}

#endif // SERVICES_INCLUDED
