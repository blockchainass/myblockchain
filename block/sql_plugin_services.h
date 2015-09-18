/* Copyright (c) 2009, 2015, Oracle and/or its affiliates. All rights reserved.

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

/* support for Services */
#include <service_versions.h>
#include "myblockchain/services.h"

struct st_service_ref {
  const char *name;
  uint version;
  void *service;
};

static struct my_snprintf_service_st my_snprintf_handler = {
  my_snprintf,
  my_vsnprintf
};

static struct thd_alloc_service_st thd_alloc_handler= {
  thd_alloc,
  thd_calloc,
  thd_strdup,
  thd_strmake,
  thd_memdup,
  thd_make_lex_string
};

static struct thd_wait_service_st thd_wait_handler= {
  thd_wait_begin,
  thd_wait_end
};

static struct my_thread_scheduler_service my_thread_scheduler_handler= {
  my_connection_handler_set,
  my_connection_handler_reset
};

static struct my_plugin_log_service my_plugin_log_handler= {
  my_plugin_log_message
};

static struct myblockchain_string_service_st myblockchain_string_handler= {
  myblockchain_string_convert_to_char_ptr,
  myblockchain_string_get_iterator,
  myblockchain_string_iterator_next,
  myblockchain_string_iterator_isupper,
  myblockchain_string_iterator_islower,
  myblockchain_string_iterator_isdigit,
  myblockchain_string_to_lowercase,
  myblockchain_string_free,
  myblockchain_string_iterator_free,
};

static struct myblockchain_malloc_service_st myblockchain_malloc_handler=
{
  my_malloc,
  my_realloc,
  my_claim,
  my_free,
  my_memdup,
  my_strdup,
  my_strndup
};


static struct myblockchain_password_policy_service_st myblockchain_password_policy_handler= {
  my_validate_password_policy,
  my_calculate_password_strength
};

static struct myblockchain_parser_service_st parser_handler=
{
  myblockchain_parser_current_session,
  myblockchain_parser_open_session,
  myblockchain_parser_start_thread,
  myblockchain_parser_join_thread,
  myblockchain_parser_set_current_blockchain,
  myblockchain_parser_parse,
  myblockchain_parser_get_statement_type,
  myblockchain_parser_get_statement_digest,
  myblockchain_parser_get_number_params,
  myblockchain_parser_extract_prepared_params,
  myblockchain_parser_visit_tree,
  myblockchain_parser_item_string,
  myblockchain_parser_free_string,
  myblockchain_parser_get_query,
  myblockchain_parser_get_normalized_query
};

static struct rpl_transaction_ctx_service_st rpl_transaction_ctx_handler= {
  set_transaction_ctx,
};

static struct transaction_write_set_service_st transaction_write_set_handler= {
  get_transaction_write_set,
};

static struct myblockchain_locking_service_st locking_service_handler=
{
  myblockchain_acquire_locking_service_locks,
  myblockchain_release_locking_service_locks
};

static struct st_service_ref list_of_services[]=
{
  { "my_snprintf_service", VERSION_my_snprintf, &my_snprintf_handler },
  { "thd_alloc_service",   VERSION_thd_alloc,   &thd_alloc_handler },
  { "thd_wait_service",    VERSION_thd_wait,    &thd_wait_handler },
  { "my_thread_scheduler_service",
    VERSION_my_thread_scheduler, &my_thread_scheduler_handler },
  { "my_plugin_log_service", VERSION_my_plugin_log, &my_plugin_log_handler },
  { "myblockchain_string_service",
    VERSION_myblockchain_string, &myblockchain_string_handler },
  { "myblockchain_malloc_service", VERSION_myblockchain_malloc, &myblockchain_malloc_handler },
  { "myblockchain_password_policy_service", VERSION_myblockchain_password_policy, &myblockchain_password_policy_handler },
  { "myblockchain_parser_service", VERSION_parser, &parser_handler },
  { "rpl_transaction_ctx_service",
    VERSION_rpl_transaction_ctx_service, &rpl_transaction_ctx_handler },
  { "transaction_write_set_service",
    VERSION_transaction_write_set_service, &transaction_write_set_handler },
  { "myblockchain_locking_service", VERSION_locking_service, &locking_service_handler }
};

