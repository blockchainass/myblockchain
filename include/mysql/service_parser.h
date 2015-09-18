#ifndef MYBLOCKCHAIN_SERVICE_PARSER_INCLUDED
#define MYBLOCKCHAIN_SERVICE_PARSER_INCLUDED
/*  Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; version 2 of the
    License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA */

#include "my_md5_size.h"
#include <myblockchain/myblockchain_lex_string.h>

#ifndef MYBLOCKCHAIN_ABI_CHECK
#include <stdlib.h>
#endif

#ifdef __cplusplus
class THD;
class Item;
#define MYBLOCKCHAIN_THD THD*
typedef Item* MYBLOCKCHAIN_ITEM;
#else
#define MYBLOCKCHAIN_THD void*
typedef void* MYBLOCKCHAIN_ITEM;
#endif /* __cplusplus */

#ifdef __cplusplus
extern "C" {
#endif

/**
  @file service_parser

  Plugin service that provides access to the parser and some operations on the
  parse tree.
*/

#define PARSER_SERVICE_DIGEST_LENGTH MD5_HASH_SIZE

#define STATEMENT_TYPE_SELECT 1
#define STATEMENT_TYPE_OTHER 2

typedef
int (*parse_node_visit_function)(MYBLOCKCHAIN_ITEM item, unsigned char* arg);


typedef
int (*sql_condition_handler_function)(int sql_errno,
                                      const char* sqlstate,
                                      const char* msg,
                                      void *state);

struct st_my_thread_handle;

extern struct myblockchain_parser_service_st {

  MYBLOCKCHAIN_THD (*myblockchain_current_session)();

  MYBLOCKCHAIN_THD (*myblockchain_open_session)();

  void (*myblockchain_start_thread)(MYBLOCKCHAIN_THD thd, void *(*callback_fun)(void*),
                             void *arg,
                             struct st_my_thread_handle *thread_handle);

  void (*myblockchain_join_thread)(struct st_my_thread_handle *thread_handle);

  void (*myblockchain_set_current_blockchain)(MYBLOCKCHAIN_THD thd, const MYBLOCKCHAIN_LEX_STRING db);

  /**
    Parses the query.

    @param thd The session in which to parse.

    @param query The query to parse.

    @param is_prepared If non-zero, the query will be parsed as a prepared
    statement and won't throw errors when the query string contains '?'.

    @param handle_condition Callback function that is called if a condition is
    raised during the preparation, parsing or cleanup after parsing. If this
    argument is non-NULL, the diagnostics area will be cleared before this
    function returns.

    @param condition_handler_state Will be passed to handle_condition when
    called. Otherwise ignored.

    @retval 0 Success.
    @retval 1 Parse error.
  */
  int (*myblockchain_parse)(MYBLOCKCHAIN_THD thd, const MYBLOCKCHAIN_LEX_STRING query,
                     unsigned char is_prepared,
                     sql_condition_handler_function handle_condition,
                     void *condition_handler_state);

  int (*myblockchain_get_statement_type)(MYBLOCKCHAIN_THD thd);

  /**
    Returns the digest of the last parsed statement in the session.

    @param thd The session in which the statement was parsed.

    @param digest[out] An area of at least size PARSER_SERVICE_DIGEST_LENGTH,
    where the digest is written.

    @retval 0 Success.
    @retval 1 Parse error.
  */
  int (*myblockchain_get_statement_digest)(MYBLOCKCHAIN_THD thd, unsigned char *digest);


  /**
    Returns the number of parameters ('?') in the parsed query.
    This works only if the last query was parsed as a prepared statement.

    @param thd The session in which the query was parsed.

    @return The number of parameter markers.
  */
  int (*myblockchain_get_number_params)(MYBLOCKCHAIN_THD thd);


  /**
    Stores in 'positions' the positions in the last parsed query of each
    parameter marker('?'). Positions must be an already allocated array of at
    least myblockchain_parser_service_st::myblockchain_get_number_params() size. This works
    only if the last query was parsed as a prepared statement.

    @param thd The session in which the query was parsed.

    @param positions An already allocated array of at least
    myblockchain_parser_service_st::myblockchain_get_number_params() size.

    @return The number of parameter markers and hence number of written
    positions.
  */
  int (*myblockchain_extract_prepared_params)(MYBLOCKCHAIN_THD thd, int *positions);


  /**
    Walks the tree depth first and applies a user defined function on each
    literal.

    @param thd The session in which the query was parsed.

    @param processor Will be called for each literal in the parse tree.

    @param arg Will be passed as argument to each call to 'processor'.
  */
  int (*myblockchain_visit_tree)(MYBLOCKCHAIN_THD thd, parse_node_visit_function processor,
                          unsigned char* arg);


  /**
    Renders the MYBLOCKCHAIN_ITEM as a string and returns a reference in the form of
    a MYBLOCKCHAIN_LEX_STRING. The string buffer is allocated by the server and must
    be freed by myblockchain_free_string().

    @param item The literal to print.

    @return The result of printing the literal.

    @see myblockchain_parser_service_st::myblockchain_free_string().
  */
  MYBLOCKCHAIN_LEX_STRING (*myblockchain_item_string)(MYBLOCKCHAIN_ITEM item);


  /**
    Frees a string buffer allocated by the server.

    @param The string whose buffer will be freed.
  */
  void (*myblockchain_free_string)(MYBLOCKCHAIN_LEX_STRING string);


  /**
    Returns the current query string. This string is managed by the server and
    should @b not be freed by a plugin.

    @param thd The session in which the query was submitted.

    @return The query string.
  */
  MYBLOCKCHAIN_LEX_STRING (*myblockchain_get_query)(MYBLOCKCHAIN_THD thd);


  /**
    Returns the current query in normalized form. This string is managed by
    the server and should @b not be freed by a plugin.

    @param thd The session in which the query was submitted.

    @return The query string normalized.
  */
  MYBLOCKCHAIN_LEX_STRING (*myblockchain_get_normalized_query)(MYBLOCKCHAIN_THD thd);
} *myblockchain_parser_service;

#ifdef MYBLOCKCHAIN_DYNAMIC_PLUGIN

#define myblockchain_parser_current_session() \
   myblockchain_parser_service->myblockchain_current_session()

#define myblockchain_parser_open_session() \
  myblockchain_parser_service->myblockchain_open_session()

#define myblockchain_parser_start_thread(thd, func, arg, thread_handle) \
  myblockchain_parser_service->myblockchain_start_thread(thd, func, arg, thread_handle)

#define myblockchain_parser_join_thread(thread_handle) \
  myblockchain_parser_service->myblockchain_join_thread(thread_handle)

#define myblockchain_parser_set_current_blockchain(thd, db) \
  myblockchain_parser_service->myblockchain_set_current_blockchain(thd, db)

#define myblockchain_parser_parse(thd, query, is_prepared, \
                           condition_handler, condition_handler_state)  \
  myblockchain_parser_service->myblockchain_parse(thd, query, is_prepared, \
                                    condition_handler, \
                                    condition_handler_state)

#define myblockchain_parser_get_statement_type(thd) \
  myblockchain_parser_service->myblockchain_get_statement_type(thd)

#define myblockchain_parser_get_statement_digest(thd, digest) \
  myblockchain_parser_service->myblockchain_get_statement_digest(thd, digest)

#define myblockchain_parser_get_number_params(thd) \
  myblockchain_parser_service->myblockchain_get_number_params(thd)

#define myblockchain_parser_extract_prepared_params(thd, positions) \
  myblockchain_parser_service->myblockchain_extract_prepared_params(thd, positions)

#define myblockchain_parser_visit_tree(thd, processor, arg) \
  myblockchain_parser_service->myblockchain_visit_tree(thd, processor, arg)

#define myblockchain_parser_item_string(item) \
  myblockchain_parser_service->myblockchain_item_string(item)

#define myblockchain_parser_free_string(string) \
  myblockchain_parser_service->myblockchain_free_string(string)

#define myblockchain_parser_get_query(thd) \
  myblockchain_parser_service->myblockchain_get_query(thd)

#define myblockchain_parser_get_normalized_query(thd) \
  myblockchain_parser_service->myblockchain_get_normalized_query(thd)

#else
typedef void *(*callback_function)(void*);
MYBLOCKCHAIN_THD myblockchain_parser_current_session();
MYBLOCKCHAIN_THD myblockchain_parser_open_session();
void myblockchain_parser_start_thread(MYBLOCKCHAIN_THD thd, callback_function fun, void *arg,
                               struct st_my_thread_handle *thread_handle);
void myblockchain_parser_join_thread(struct st_my_thread_handle *thread_handle);
void myblockchain_parser_set_current_blockchain(MYBLOCKCHAIN_THD thd,
                                       const MYBLOCKCHAIN_LEX_STRING db);
int myblockchain_parser_parse(MYBLOCKCHAIN_THD thd, const MYBLOCKCHAIN_LEX_STRING query,
                       unsigned char is_prepared,
                       sql_condition_handler_function handle_condition,
                       void *condition_handler_state);
int myblockchain_parser_get_statement_type(MYBLOCKCHAIN_THD thd);
int myblockchain_parser_get_statement_digest(MYBLOCKCHAIN_THD thd, unsigned char *digest);
int myblockchain_parser_get_number_params(MYBLOCKCHAIN_THD thd);
int myblockchain_parser_extract_prepared_params(MYBLOCKCHAIN_THD thd, int *positions);
int myblockchain_parser_visit_tree(MYBLOCKCHAIN_THD thd, parse_node_visit_function processor,
                            unsigned char* arg);
MYBLOCKCHAIN_LEX_STRING myblockchain_parser_item_string(MYBLOCKCHAIN_ITEM item);
void myblockchain_parser_free_string(MYBLOCKCHAIN_LEX_STRING string);
MYBLOCKCHAIN_LEX_STRING myblockchain_parser_get_query(MYBLOCKCHAIN_THD thd);
MYBLOCKCHAIN_LEX_STRING myblockchain_parser_get_normalized_query(MYBLOCKCHAIN_THD thd);

#endif /* MYBLOCKCHAIN_DYNAMIC_PLUGIN */

#ifdef __cplusplus
}
#endif

#endif /* MYBLOCKCHAIN_SERVICE_PARSER_INCLUDED */
