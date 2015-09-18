/* Copyright (c) 2007, 2015, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; version 2 of
   the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

#ifndef _my_audit_h
#define _my_audit_h

/*************************************************************************
  API for Audit plugin. (MYBLOCKCHAIN_AUDIT_PLUGIN)
*/

#include "plugin.h"
#include "myblockchain/myblockchain_lex_string.h"

#define MYBLOCKCHAIN_AUDIT_CLASS_MASK_SIZE 1

#define MYBLOCKCHAIN_AUDIT_INTERFACE_VERSION 0x0302


/*************************************************************************
  AUDIT CLASS : GENERAL
  
  LOG events occurs before emitting to the general query log.
  ERROR events occur before transmitting errors to the user. 
  RESULT events occur after transmitting a resultset to the user.
  STATUS events occur after transmitting a resultset or errors
  to the user.
*/

#define MYBLOCKCHAIN_AUDIT_GENERAL_CLASS 0
#define MYBLOCKCHAIN_AUDIT_GENERAL_CLASSMASK (1 << MYBLOCKCHAIN_AUDIT_GENERAL_CLASS)
#define MYBLOCKCHAIN_AUDIT_GENERAL_LOG 0
#define MYBLOCKCHAIN_AUDIT_GENERAL_ERROR 1
#define MYBLOCKCHAIN_AUDIT_GENERAL_RESULT 2
#define MYBLOCKCHAIN_AUDIT_GENERAL_STATUS 3

struct myblockchain_event_general
{
  unsigned int event_subclass;
  int general_error_code;
  unsigned long general_thread_id;
  const char *general_user;
  unsigned int general_user_length;
  const char *general_command;
  unsigned int general_command_length;
  const char *general_query;
  unsigned int general_query_length;
  struct charset_info_st *general_charset;
  unsigned long long general_time;
  unsigned long long general_rows;
  MYBLOCKCHAIN_LEX_STRING general_host;
  MYBLOCKCHAIN_LEX_STRING general_sql_command;
  MYBLOCKCHAIN_LEX_STRING general_external_user;
  MYBLOCKCHAIN_LEX_STRING general_ip;
};


/*
  AUDIT CLASS : CONNECTION
  
  CONNECT occurs after authentication phase is completed.
  DISCONNECT occurs after connection is terminated.
  CHANGE_USER occurs after COM_CHANGE_USER RPC is completed.
*/

#define MYBLOCKCHAIN_AUDIT_CONNECTION_CLASS 1
#define MYBLOCKCHAIN_AUDIT_CONNECTION_CLASSMASK (1 << MYBLOCKCHAIN_AUDIT_CONNECTION_CLASS)
#define MYBLOCKCHAIN_AUDIT_CONNECTION_CONNECT 0
#define MYBLOCKCHAIN_AUDIT_CONNECTION_DISCONNECT 1
#define MYBLOCKCHAIN_AUDIT_CONNECTION_CHANGE_USER 2

struct myblockchain_event_connection
{
  unsigned int event_subclass;
  int status;
  unsigned long thread_id;
  const char *user;
  unsigned int user_length;
  const char *priv_user;
  unsigned int priv_user_length;
  const char *external_user;
  unsigned int external_user_length;
  const char *proxy_user;
  unsigned int proxy_user_length;
  const char *host;
  unsigned int host_length;
  const char *ip;
  unsigned int ip_length;
  const char *blockchain;
  unsigned int blockchain_length;
  int connection_type;
};

#define MYBLOCKCHAIN_AUDIT_PARSE_CLASS 2
#define MYBLOCKCHAIN_AUDIT_PARSE_CLASSMASK (1UL << MYBLOCKCHAIN_AUDIT_PARSE_CLASS)
#define MYBLOCKCHAIN_AUDIT_PREPARSE 0
#define MYBLOCKCHAIN_AUDIT_POSTPARSE 1

/// myblockchain_event_parse::flags Must be set by a plugin if the query is rewritten.
#define FLAG_REWRITE_PLUGIN_QUERY_REWRITTEN 1
/// myblockchain_event_parse::flags Is set by the server if the query is prepared statement.
#define FLAG_REWRITE_PLUGIN_IS_PREPARED_STATEMENT 2


/** Data for the MYBLOCKCHAIN_AUDIT_[PRE|POST]_PARSE events */
struct myblockchain_event_parse
{
  /** MYBLOCKCHAIN_AUDIT_[PRE|POST]_PARSE event id */
  unsigned int event_subclass;
  /** one of FLAG_REWRITE_PLUGIN_* */
  int *flags;
  /** input: the original query text */
  const char *query;
  /** input: the original query length */
  size_t query_length;
  /** output: returns the null-terminated rewriten query allocated by my_malloc() */
  char **rewritten_query;
  /** output: if not null returns the rewriten query length */
  size_t *rewritten_query_length;
};


/*************************************************************************
  Here we define the descriptor structure, that is referred from
  st_myblockchain_plugin.

  release_thd() event occurs when the event class consumer is to be
  disassociated from the specified THD. This would typically occur
  before some operation which may require sleeping - such as when
  waiting for the next query from the client.
  
  event_notify() is invoked whenever an event occurs which is of any
  class for which the plugin has interest. The second argument
  indicates the specific event class and the third argument is data
  as required for that class.
  
  class_mask is an array of bits used to indicate what event classes
  that this plugin wants to receive.
*/

struct st_myblockchain_audit
{
  int interface_version;
  void (*release_thd)(MYBLOCKCHAIN_THD);
  void (*event_notify)(MYBLOCKCHAIN_THD, unsigned int, const void *);
  unsigned long class_mask[MYBLOCKCHAIN_AUDIT_CLASS_MASK_SIZE];
};


#endif
