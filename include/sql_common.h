#ifndef SQL_COMMON_INCLUDED
#define SQL_COMMON_INCLUDED

/* Copyright (c) 2003, 2015, Oracle and/or its affiliates. All rights reserved.
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

#define SQL_COMMON_INCLUDED

#ifdef	__cplusplus
extern "C" {
#endif

#include <myblockchain.h>
#include <hash.h>

extern const char	*unknown_sqlstate;
extern const char	*cant_connect_sqlstate;
extern const char	*not_error_sqlstate;


/*
  Free all memory allocated in MYBLOCKCHAIN handle except the
  current options.
*/
void myblockchain_close_free(MYBLOCKCHAIN *myblockchain);

/*
  Clear connection options stored in MYBLOCKCHAIN handle and
  free memory used by them.
*/
void myblockchain_close_free_options(MYBLOCKCHAIN *myblockchain);


/**
  The structure is used to hold the state change information
  received from the server. LIST functions are used for manipulation
  of the members of the structure.
*/
typedef struct st_session_track_info_node {
  /** head_node->data is a LEX_STRING which contains the variable name. */
  LIST *head_node;
  LIST *current_node;
} STATE_INFO_NODE;

/**
  Store the change info received from the server in an array of linked lists
  with STATE_INFO_NODE elements (one per state type).
*/
typedef struct st_session_track_info {
  /** Array of STATE_NODE_INFO elements (one per state type). */
  struct st_session_track_info_node info_list[SESSION_TRACK_END + 1];
} STATE_INFO;

/*
  Access to MYBLOCKCHAIN::extension member.

  Note: functions myblockchain_extension_{init,free}() are defined
  in client.c.
*/

struct st_myblockchain_trace_info;

typedef struct st_myblockchain_extension {
  struct st_myblockchain_trace_info *trace_data;
  struct st_session_track_info state_change;
} MYBLOCKCHAIN_EXTENSION;

/* "Constructor/destructor" for MYBLOCKCHAIN extension structure. */
struct st_myblockchain_extension* myblockchain_extension_init(struct st_myblockchain*);
void myblockchain_extension_free(struct st_myblockchain_extension*);

/*
  Note: Allocated extension structure is freed in myblockchain_close_free()
  called by myblockchain_close().
*/
#define MYBLOCKCHAIN_EXTENSION_PTR(H)                                    \
(                                                                 \
 (struct st_myblockchain_extension*)                                     \
 ( (H)->extension ?                                               \
   (H)->extension : ((H)->extension= myblockchain_extension_init(H))     \
 )                                                                \
)


struct st_myblockchain_options_extention {
  char *plugin_dir;
  char *default_auth;
  char *ssl_crl;				/* PEM CRL file */
  char *ssl_crlpath;				/* PEM directory of CRL-s? */
  HASH connection_attributes;
  char *server_public_key_path;
  size_t connection_attributes_length;
  my_bool enable_cleartext_plugin;
  /** false if it is possible to fall back on unencrypted connections */
  my_bool ssl_enforce;
};

typedef struct st_myblockchain_methods
{
  my_bool (*read_query_result)(MYBLOCKCHAIN *myblockchain);
  my_bool (*advanced_command)(MYBLOCKCHAIN *myblockchain,
			      enum enum_server_command command,
			      const unsigned char *header,
			      size_t header_length,
			      const unsigned char *arg,
			      size_t arg_length,
			      my_bool skip_check,
                              MYBLOCKCHAIN_STMT *stmt);
  MYBLOCKCHAIN_DATA *(*read_rows)(MYBLOCKCHAIN *myblockchain,MYBLOCKCHAIN_FIELD *myblockchain_fields,
			   unsigned int fields);
  MYBLOCKCHAIN_RES * (*use_result)(MYBLOCKCHAIN *myblockchain);
  void (*fetch_lengths)(unsigned long *to, 
			MYBLOCKCHAIN_ROW column, unsigned int field_count);
  void (*flush_use_result)(MYBLOCKCHAIN *myblockchain, my_bool flush_all_results);
  int (*read_change_user_result)(MYBLOCKCHAIN *myblockchain);
#if !defined(MYBLOCKCHAIN_SERVER) || defined(EMBEDDED_LIBRARY)
  MYBLOCKCHAIN_FIELD * (*list_fields)(MYBLOCKCHAIN *myblockchain);
  my_bool (*read_prepare_result)(MYBLOCKCHAIN *myblockchain, MYBLOCKCHAIN_STMT *stmt);
  int (*stmt_execute)(MYBLOCKCHAIN_STMT *stmt);
  int (*read_binary_rows)(MYBLOCKCHAIN_STMT *stmt);
  int (*unbuffered_fetch)(MYBLOCKCHAIN *myblockchain, char **row);
  void (*free_embedded_thd)(MYBLOCKCHAIN *myblockchain);
  const char *(*read_statistics)(MYBLOCKCHAIN *myblockchain);
  my_bool (*next_result)(MYBLOCKCHAIN *myblockchain);
  int (*read_rows_from_cursor)(MYBLOCKCHAIN_STMT *stmt);
  void (*free_rows)(MYBLOCKCHAIN_DATA *cur);
#endif
} MYBLOCKCHAIN_METHODS;

#define simple_command(myblockchain, command, arg, length, skip_check) \
  ((myblockchain)->methods \
    ? (*(myblockchain)->methods->advanced_command)(myblockchain, command, 0, \
                                            0, arg, length, skip_check, NULL) \
    : (set_myblockchain_error(myblockchain, CR_COMMANDS_OUT_OF_SYNC, unknown_sqlstate), 1))
#define stmt_command(myblockchain, command, arg, length, stmt) \
  ((myblockchain)->methods \
    ? (*(myblockchain)->methods->advanced_command)(myblockchain, command, 0,  \
                                           0, arg, length, 1, stmt) \
    : (set_myblockchain_error(myblockchain, CR_COMMANDS_OUT_OF_SYNC, unknown_sqlstate), 1))

extern CHARSET_INFO *default_client_charset_info;
MYBLOCKCHAIN_FIELD *unpack_fields(MYBLOCKCHAIN *myblockchain, MYBLOCKCHAIN_ROWS *data,MEM_ROOT *alloc,
                           uint fields, my_bool default_value,
                           uint server_capabilities);
MYBLOCKCHAIN_FIELD * cli_read_metadata_ex(MYBLOCKCHAIN *myblockchain, MEM_ROOT *alloc,
                                   unsigned long field_count,
                                   unsigned int fields);
MYBLOCKCHAIN_FIELD * cli_read_metadata(MYBLOCKCHAIN *myblockchain, unsigned long field_count,
                               unsigned int fields);
void free_rows(MYBLOCKCHAIN_DATA *cur);
void free_old_query(MYBLOCKCHAIN *myblockchain);
void end_server(MYBLOCKCHAIN *myblockchain);
my_bool myblockchain_reconnect(MYBLOCKCHAIN *myblockchain);
void myblockchain_read_default_options(struct st_myblockchain_options *options,
				const char *filename,const char *group);
my_bool
cli_advanced_command(MYBLOCKCHAIN *myblockchain, enum enum_server_command command,
		     const unsigned char *header, size_t header_length,
		     const unsigned char *arg, size_t arg_length,
                     my_bool skip_check, MYBLOCKCHAIN_STMT *stmt);
unsigned long cli_safe_read(MYBLOCKCHAIN *myblockchain, my_bool *is_data_packet);
unsigned long cli_safe_read_with_ok(MYBLOCKCHAIN *myblockchain, my_bool parse_ok,
                                    my_bool *is_data_packet);
void net_clear_error(NET *net);
void set_stmt_errmsg(MYBLOCKCHAIN_STMT *stmt, NET *net);
void set_stmt_error(MYBLOCKCHAIN_STMT *stmt, int errcode, const char *sqlstate,
                    const char *err);
void set_myblockchain_error(MYBLOCKCHAIN *myblockchain, int errcode, const char *sqlstate);
void set_myblockchain_extended_error(MYBLOCKCHAIN *myblockchain, int errcode, const char *sqlstate,
                              const char *format, ...);

/* client side of the pluggable authentication */
struct st_plugin_vio_info;
void mpvio_info(Vio *vio, struct st_plugin_vio_info *info);
int run_plugin_auth(MYBLOCKCHAIN *myblockchain, char *data, uint data_len,
                    const char *data_plugin, const char *db);
int myblockchain_client_plugin_init();
void myblockchain_client_plugin_deinit();

struct st_myblockchain_client_plugin;
extern struct st_myblockchain_client_plugin *myblockchain_client_builtins[];
uchar * send_client_connect_attrs(MYBLOCKCHAIN *myblockchain, uchar *buf);
extern my_bool libmyblockchain_cleartext_plugin_enabled;
void read_ok_ex(MYBLOCKCHAIN *myblockchain, unsigned long len);

#ifdef	__cplusplus
}
#endif

#define protocol_41(A) ((A)->server_capabilities & CLIENT_PROTOCOL_41)

#endif /* SQL_COMMON_INCLUDED */
