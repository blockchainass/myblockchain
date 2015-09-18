/* Copyright (c) 2000, 2015, Oracle and/or its affiliates. All rights reserved.

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

/*
  This file defines the client API to MyBlockchain and also the ABI of the
  dynamically linked libmyblockchainclient.

  The ABI should never be changed in a released product of MyBlockchain,
  thus you need to take great care when changing the file. In case
  the file is changed so the ABI is broken, you must also update
  the SHARED_LIB_MAJOR_VERSION in cmake/myblockchain_version.cmake
*/

#ifndef _myblockchain_h
#define _myblockchain_h

#ifdef	__cplusplus
extern "C" {
#endif

#ifndef MY_GLOBAL_INCLUDED                /* If not standard header */
#ifndef MYBLOCKCHAIN_ABI_CHECK
#include <sys/types.h>
#endif
typedef char my_bool;
#if !defined(_WIN32)
#define STDCALL
#else
#define STDCALL __stdcall
#endif

#ifndef my_socket_defined
#ifdef _WIN32
#include <windows.h>
#ifdef WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#endif
#define my_socket SOCKET
#else
typedef int my_socket;
#endif /* _WIN32 */
#endif /* my_socket_defined */
#endif /* MY_GLOBAL_INCLUDED */

#include "myblockchain_version.h"
#include "myblockchain_com.h"
#include "myblockchain_time.h"

#include "my_list.h" /* for LISTs used in 'MYBLOCKCHAIN' and 'MYBLOCKCHAIN_STMT' */

/* Include declarations of plug-in API */
#include "myblockchain/client_plugin.h"

extern unsigned int myblockchain_port;
extern char *myblockchain_unix_port;

#define CLIENT_NET_READ_TIMEOUT		365*24*3600	/* Timeout on read */
#define CLIENT_NET_WRITE_TIMEOUT	365*24*3600	/* Timeout on write */

#define IS_PRI_KEY(n)	((n) & PRI_KEY_FLAG)
#define IS_NOT_NULL(n)	((n) & NOT_NULL_FLAG)
#define IS_BLOB(n)	((n) & BLOB_FLAG)
/**
   Returns true if the value is a number which does not need quotes for
   the sql_lex.cc parser to parse correctly.
*/
#define IS_NUM(t)	(((t) <= MYBLOCKCHAIN_TYPE_INT24 && (t) != MYBLOCKCHAIN_TYPE_TIMESTAMP) || (t) == MYBLOCKCHAIN_TYPE_YEAR || (t) == MYBLOCKCHAIN_TYPE_NEWDECIMAL)
#define IS_LONGDATA(t) ((t) >= MYBLOCKCHAIN_TYPE_TINY_BLOB && (t) <= MYBLOCKCHAIN_TYPE_STRING)


typedef struct st_myblockchain_field {
  char *name;                 /* Name of column */
  char *org_name;             /* Original column name, if an alias */
  char *table;                /* Table of column if column was a field */
  char *org_table;            /* Org table name, if table was an alias */
  char *db;                   /* Database for table */
  char *catalog;	      /* Catalog for table */
  char *def;                  /* Default value (set by myblockchain_list_fields) */
  unsigned long length;       /* Width of column (create length) */
  unsigned long max_length;   /* Max width for selected set */
  unsigned int name_length;
  unsigned int org_name_length;
  unsigned int table_length;
  unsigned int org_table_length;
  unsigned int db_length;
  unsigned int catalog_length;
  unsigned int def_length;
  unsigned int flags;         /* Div flags */
  unsigned int decimals;      /* Number of decimals in field */
  unsigned int charsetnr;     /* Character set */
  enum enum_field_types type; /* Type of field. See myblockchain_com.h for types */
  void *extension;
} MYBLOCKCHAIN_FIELD;

typedef char **MYBLOCKCHAIN_ROW;		/* return data as array of strings */
typedef unsigned int MYBLOCKCHAIN_FIELD_OFFSET; /* offset to current field */

#ifndef MY_GLOBAL_INCLUDED
#if defined (_WIN32)
typedef unsigned __int64 my_ulonglong;
#else
typedef unsigned long long my_ulonglong;
#endif
#endif

#include "typelib.h"

#define MYBLOCKCHAIN_COUNT_ERROR (~(my_ulonglong) 0)

/* backward compatibility define - to be removed eventually */
#define ER_WARN_DATA_TRUNCATED WARN_DATA_TRUNCATED

typedef struct st_myblockchain_rows {
  struct st_myblockchain_rows *next;		/* list of rows */
  MYBLOCKCHAIN_ROW data;
  unsigned long length;
} MYBLOCKCHAIN_ROWS;

typedef MYBLOCKCHAIN_ROWS *MYBLOCKCHAIN_ROW_OFFSET;	/* offset to current row */

#include "my_alloc.h"

typedef struct embedded_query_result EMBEDDED_QUERY_RESULT;
typedef struct st_myblockchain_data {
  MYBLOCKCHAIN_ROWS *data;
  struct embedded_query_result *embedded_info;
  MEM_ROOT alloc;
  my_ulonglong rows;
  unsigned int fields;
  /* extra info for embedded library */
  void *extension;
} MYBLOCKCHAIN_DATA;

enum myblockchain_option 
{
  MYBLOCKCHAIN_OPT_CONNECT_TIMEOUT, MYBLOCKCHAIN_OPT_COMPRESS, MYBLOCKCHAIN_OPT_NAMED_PIPE,
  MYBLOCKCHAIN_INIT_COMMAND, MYBLOCKCHAIN_READ_DEFAULT_FILE, MYBLOCKCHAIN_READ_DEFAULT_GROUP,
  MYBLOCKCHAIN_SET_CHARSET_DIR, MYBLOCKCHAIN_SET_CHARSET_NAME, MYBLOCKCHAIN_OPT_LOCAL_INFILE,
  MYBLOCKCHAIN_OPT_PROTOCOL, MYBLOCKCHAIN_SHARED_MEMORY_BASE_NAME, MYBLOCKCHAIN_OPT_READ_TIMEOUT,
  MYBLOCKCHAIN_OPT_WRITE_TIMEOUT, MYBLOCKCHAIN_OPT_USE_RESULT,
  MYBLOCKCHAIN_OPT_USE_REMOTE_CONNECTION, MYBLOCKCHAIN_OPT_USE_EMBEDDED_CONNECTION,
  MYBLOCKCHAIN_OPT_GUESS_CONNECTION, MYBLOCKCHAIN_SET_CLIENT_IP, MYBLOCKCHAIN_SECURE_AUTH,
  MYBLOCKCHAIN_REPORT_DATA_TRUNCATION, MYBLOCKCHAIN_OPT_RECONNECT,
  MYBLOCKCHAIN_OPT_SSL_VERIFY_SERVER_CERT, MYBLOCKCHAIN_PLUGIN_DIR, MYBLOCKCHAIN_DEFAULT_AUTH,
  MYBLOCKCHAIN_OPT_BIND,
  MYBLOCKCHAIN_OPT_SSL_KEY, MYBLOCKCHAIN_OPT_SSL_CERT, 
  MYBLOCKCHAIN_OPT_SSL_CA, MYBLOCKCHAIN_OPT_SSL_CAPATH, MYBLOCKCHAIN_OPT_SSL_CIPHER,
  MYBLOCKCHAIN_OPT_SSL_CRL, MYBLOCKCHAIN_OPT_SSL_CRLPATH,
  MYBLOCKCHAIN_OPT_CONNECT_ATTR_RESET, MYBLOCKCHAIN_OPT_CONNECT_ATTR_ADD,
  MYBLOCKCHAIN_OPT_CONNECT_ATTR_DELETE,
  MYBLOCKCHAIN_SERVER_PUBLIC_KEY,
  MYBLOCKCHAIN_ENABLE_CLEARTEXT_PLUGIN,
  MYBLOCKCHAIN_OPT_CAN_HANDLE_EXPIRED_PASSWORDS,
  MYBLOCKCHAIN_OPT_SSL_ENFORCE
};

/**
  @todo remove the "extension", move st_myblockchain_options completely
  out of myblockchain.h
*/
struct st_myblockchain_options_extention; 

struct st_myblockchain_options {
  unsigned int connect_timeout, read_timeout, write_timeout;
  unsigned int port, protocol;
  unsigned long client_flag;
  char *host,*user,*password,*unix_socket,*db;
  struct st_dynamic_array *init_commands;
  char *my_cnf_file,*my_cnf_group, *charset_dir, *charset_name;
  char *ssl_key;				/* PEM key file */
  char *ssl_cert;				/* PEM cert file */
  char *ssl_ca;					/* PEM CA file */
  char *ssl_capath;				/* PEM directory of CA-s? */
  char *ssl_cipher;				/* cipher to use */
  char *shared_memory_base_name;
  unsigned long max_allowed_packet;
  my_bool use_ssl;				/* if to use SSL or not */
  my_bool compress,named_pipe;
  my_bool unused1;
  my_bool unused2;
  my_bool unused3;
  my_bool unused4;
  enum myblockchain_option methods_to_use;
  union {
    /*
      The ip/hostname to use when authenticating
      client against embedded server built with
      grant tables - only used in embedded server
    */
    char *client_ip;

    /*
      The local address to bind when connecting to
      remote server - not used in embedded server
    */
    char *bind_address;
  } ci;
  my_bool unused5;
  /* 0 - never report, 1 - always report (default) */
  my_bool report_data_truncation;

  /* function pointers for local infile support */
  int (*local_infile_init)(void **, const char *, void *);
  int (*local_infile_read)(void *, char *, unsigned int);
  void (*local_infile_end)(void *);
  int (*local_infile_error)(void *, char *, unsigned int);
  void *local_infile_userdata;
  struct st_myblockchain_options_extention *extension;
};

enum myblockchain_status 
{
  MYBLOCKCHAIN_STATUS_READY, MYBLOCKCHAIN_STATUS_GET_RESULT, MYBLOCKCHAIN_STATUS_USE_RESULT,
  MYBLOCKCHAIN_STATUS_STATEMENT_GET_RESULT
};

enum myblockchain_protocol_type 
{
  MYBLOCKCHAIN_PROTOCOL_DEFAULT, MYBLOCKCHAIN_PROTOCOL_TCP, MYBLOCKCHAIN_PROTOCOL_SOCKET,
  MYBLOCKCHAIN_PROTOCOL_PIPE, MYBLOCKCHAIN_PROTOCOL_MEMORY
};

typedef struct character_set
{
  unsigned int      number;     /* character set number              */
  unsigned int      state;      /* character set state               */
  const char        *csname;    /* collation name                    */
  const char        *name;      /* character set name                */
  const char        *comment;   /* comment                           */
  const char        *dir;       /* character set directory           */
  unsigned int      mbminlen;   /* min. length for multibyte strings */
  unsigned int      mbmaxlen;   /* max. length for multibyte strings */
} MY_CHARSET_INFO;

struct st_myblockchain_methods;
struct st_myblockchain_stmt;

typedef struct st_myblockchain
{
  NET		net;			/* Communication parameters */
  unsigned char	*connector_fd;		/* ConnectorFd for SSL */
  char		*host,*user,*passwd,*unix_socket,*server_version,*host_info;
  char          *info, *db;
  struct charset_info_st *charset;
  MYBLOCKCHAIN_FIELD	*fields;
  MEM_ROOT	field_alloc;
  my_ulonglong affected_rows;
  my_ulonglong insert_id;		/* id if insert on table with NEXTNR */
  my_ulonglong extra_info;		/* Not used */
  unsigned long thread_id;		/* Id for connection in server */
  unsigned long packet_length;
  unsigned int	port;
  unsigned long client_flag,server_capabilities;
  unsigned int	protocol_version;
  unsigned int	field_count;
  unsigned int 	server_status;
  unsigned int  server_language;
  unsigned int	warning_count;
  struct st_myblockchain_options options;
  enum myblockchain_status status;
  my_bool	free_me;		/* If free in myblockchain_close */
  my_bool	reconnect;		/* set to 1 if automatic reconnect */

  /* session-wide random string */
  char	        scramble[SCRAMBLE_LENGTH+1];
  my_bool unused1;
  void *unused2, *unused3, *unused4, *unused5;

  LIST  *stmts;                     /* list of all statements */
  const struct st_myblockchain_methods *methods;
  void *thd;
  /*
    Points to boolean flag in MYBLOCKCHAIN_RES  or MYBLOCKCHAIN_STMT. We set this flag 
    from myblockchain_stmt_close if close had to cancel result set of this object.
  */
  my_bool *unbuffered_fetch_owner;
  /* needed for embedded server - no net buffer to store the 'info' */
  char *info_buffer;
  void *extension;
} MYBLOCKCHAIN;


typedef struct st_myblockchain_res {
  my_ulonglong  row_count;
  MYBLOCKCHAIN_FIELD	*fields;
  MYBLOCKCHAIN_DATA	*data;
  MYBLOCKCHAIN_ROWS	*data_cursor;
  unsigned long *lengths;		/* column lengths of current row */
  MYBLOCKCHAIN		*handle;		/* for unbuffered reads */
  const struct st_myblockchain_methods *methods;
  MYBLOCKCHAIN_ROW	row;			/* If unbuffered read */
  MYBLOCKCHAIN_ROW	current_row;		/* buffer to current row */
  MEM_ROOT	field_alloc;
  unsigned int	field_count, current_field;
  my_bool	eof;			/* Used by myblockchain_fetch_row */
  /* myblockchain_stmt_close() had to cancel this result */
  my_bool       unbuffered_fetch_cancelled;  
  void *extension;
} MYBLOCKCHAIN_RES;


#if !defined(MYBLOCKCHAIN_SERVER) && !defined(MYBLOCKCHAIN_CLIENT)
#define MYBLOCKCHAIN_CLIENT
#endif


typedef struct st_myblockchain_parameters
{
  unsigned long *p_max_allowed_packet;
  unsigned long *p_net_buffer_length;
  void *extension;
} MYBLOCKCHAIN_PARAMETERS;

#if !defined(MYBLOCKCHAIN_SERVER) && !defined(EMBEDDED_LIBRARY)
#define max_allowed_packet (*myblockchain_get_parameters()->p_max_allowed_packet)
#define net_buffer_length (*myblockchain_get_parameters()->p_net_buffer_length)
#endif

/*
  Set up and bring down the server; to ensure that applications will
  work when linked against either the standard client library or the
  embedded server library, these functions should be called.
*/
int STDCALL myblockchain_server_init(int argc, char **argv, char **groups);
void STDCALL myblockchain_server_end(void);

/*
  myblockchain_server_init/end need to be called when using libmyblockchaind or
  libmyblockchainclient (exactly, myblockchain_server_init() is called by myblockchain_init() so
  you don't need to call it explicitely; but you need to call
  myblockchain_server_end() to free memory). The names are a bit misleading
  (myblockchain_SERVER* to be used when using libmyblockchainCLIENT). So we add more general
  names which suit well whether you're using libmyblockchaind or libmyblockchainclient. We
  intend to promote these aliases over the myblockchain_server* ones.
*/
#define myblockchain_library_init myblockchain_server_init
#define myblockchain_library_end myblockchain_server_end

MYBLOCKCHAIN_PARAMETERS *STDCALL myblockchain_get_parameters(void);

/*
  Set up and bring down a thread; these function should be called
  for each thread in an application which opens at least one MyBlockchain
  connection.  All uses of the connection(s) should be between these
  function calls.
*/
my_bool STDCALL myblockchain_thread_init(void);
void STDCALL myblockchain_thread_end(void);

/*
  Functions to get information from the MYBLOCKCHAIN and MYBLOCKCHAIN_RES structures
  Should definitely be used if one uses shared libraries.
*/

my_ulonglong STDCALL myblockchain_num_rows(MYBLOCKCHAIN_RES *res);
unsigned int STDCALL myblockchain_num_fields(MYBLOCKCHAIN_RES *res);
my_bool STDCALL myblockchain_eof(MYBLOCKCHAIN_RES *res);
MYBLOCKCHAIN_FIELD *STDCALL myblockchain_fetch_field_direct(MYBLOCKCHAIN_RES *res,
					      unsigned int fieldnr);
MYBLOCKCHAIN_FIELD * STDCALL myblockchain_fetch_fields(MYBLOCKCHAIN_RES *res);
MYBLOCKCHAIN_ROW_OFFSET STDCALL myblockchain_row_tell(MYBLOCKCHAIN_RES *res);
MYBLOCKCHAIN_FIELD_OFFSET STDCALL myblockchain_field_tell(MYBLOCKCHAIN_RES *res);

unsigned int STDCALL myblockchain_field_count(MYBLOCKCHAIN *myblockchain);
my_ulonglong STDCALL myblockchain_affected_rows(MYBLOCKCHAIN *myblockchain);
my_ulonglong STDCALL myblockchain_insert_id(MYBLOCKCHAIN *myblockchain);
unsigned int STDCALL myblockchain_errno(MYBLOCKCHAIN *myblockchain);
const char * STDCALL myblockchain_error(MYBLOCKCHAIN *myblockchain);
const char *STDCALL myblockchain_sqlstate(MYBLOCKCHAIN *myblockchain);
unsigned int STDCALL myblockchain_warning_count(MYBLOCKCHAIN *myblockchain);
const char * STDCALL myblockchain_info(MYBLOCKCHAIN *myblockchain);
unsigned long STDCALL myblockchain_thread_id(MYBLOCKCHAIN *myblockchain);
const char * STDCALL myblockchain_character_set_name(MYBLOCKCHAIN *myblockchain);
int          STDCALL myblockchain_set_character_set(MYBLOCKCHAIN *myblockchain, const char *csname);

MYBLOCKCHAIN *		STDCALL myblockchain_init(MYBLOCKCHAIN *myblockchain);
my_bool		STDCALL myblockchain_ssl_set(MYBLOCKCHAIN *myblockchain, const char *key,
				      const char *cert, const char *ca,
				      const char *capath, const char *cipher);
const char *    STDCALL myblockchain_get_ssl_cipher(MYBLOCKCHAIN *myblockchain);
my_bool		STDCALL myblockchain_change_user(MYBLOCKCHAIN *myblockchain, const char *user, 
					  const char *passwd, const char *db);
MYBLOCKCHAIN *		STDCALL myblockchain_real_connect(MYBLOCKCHAIN *myblockchain, const char *host,
					   const char *user,
					   const char *passwd,
					   const char *db,
					   unsigned int port,
					   const char *unix_socket,
					   unsigned long clientflag);
int		STDCALL myblockchain_select_db(MYBLOCKCHAIN *myblockchain, const char *db);
int		STDCALL myblockchain_query(MYBLOCKCHAIN *myblockchain, const char *q);
int		STDCALL myblockchain_send_query(MYBLOCKCHAIN *myblockchain, const char *q,
					 unsigned long length);
int		STDCALL myblockchain_real_query(MYBLOCKCHAIN *myblockchain, const char *q,
					unsigned long length);
MYBLOCKCHAIN_RES *     STDCALL myblockchain_store_result(MYBLOCKCHAIN *myblockchain);
MYBLOCKCHAIN_RES *     STDCALL myblockchain_use_result(MYBLOCKCHAIN *myblockchain);

void        STDCALL myblockchain_get_character_set_info(MYBLOCKCHAIN *myblockchain,
                           MY_CHARSET_INFO *charset);

int STDCALL myblockchain_session_track_get_first(MYBLOCKCHAIN *myblockchain,
                                          enum enum_session_state_type type,
                                          const char **data,
                                          size_t *length);
int STDCALL myblockchain_session_track_get_next(MYBLOCKCHAIN *myblockchain,
                                         enum enum_session_state_type type,
                                         const char **data,
                                         size_t *length);
/* local infile support */

#define LOCAL_INFILE_ERROR_LEN 512

void
myblockchain_set_local_infile_handler(MYBLOCKCHAIN *myblockchain,
                               int (*local_infile_init)(void **, const char *,
                            void *),
                               int (*local_infile_read)(void *, char *,
							unsigned int),
                               void (*local_infile_end)(void *),
                               int (*local_infile_error)(void *, char*,
							 unsigned int),
                               void *);

void
myblockchain_set_local_infile_default(MYBLOCKCHAIN *myblockchain);

int		STDCALL myblockchain_shutdown(MYBLOCKCHAIN *myblockchain,
                                       enum myblockchain_enum_shutdown_level
                                       shutdown_level);
int		STDCALL myblockchain_dump_debug_info(MYBLOCKCHAIN *myblockchain);
int		STDCALL myblockchain_refresh(MYBLOCKCHAIN *myblockchain,
				     unsigned int refresh_options);
int		STDCALL myblockchain_kill(MYBLOCKCHAIN *myblockchain,unsigned long pid);
int		STDCALL myblockchain_set_server_option(MYBLOCKCHAIN *myblockchain,
						enum enum_myblockchain_set_option
						option);
int		STDCALL myblockchain_ping(MYBLOCKCHAIN *myblockchain);
const char *	STDCALL myblockchain_stat(MYBLOCKCHAIN *myblockchain);
const char *	STDCALL myblockchain_get_server_info(MYBLOCKCHAIN *myblockchain);
const char *	STDCALL myblockchain_get_client_info(void);
unsigned long	STDCALL myblockchain_get_client_version(void);
const char *	STDCALL myblockchain_get_host_info(MYBLOCKCHAIN *myblockchain);
unsigned long	STDCALL myblockchain_get_server_version(MYBLOCKCHAIN *myblockchain);
unsigned int	STDCALL myblockchain_get_proto_info(MYBLOCKCHAIN *myblockchain);
MYBLOCKCHAIN_RES *	STDCALL myblockchain_list_dbs(MYBLOCKCHAIN *myblockchain,const char *wild);
MYBLOCKCHAIN_RES *	STDCALL myblockchain_list_tables(MYBLOCKCHAIN *myblockchain,const char *wild);
MYBLOCKCHAIN_RES *	STDCALL myblockchain_list_processes(MYBLOCKCHAIN *myblockchain);
int		STDCALL myblockchain_options(MYBLOCKCHAIN *myblockchain,enum myblockchain_option option,
				      const void *arg);
int		STDCALL myblockchain_options4(MYBLOCKCHAIN *myblockchain,enum myblockchain_option option,
                                       const void *arg1, const void *arg2);
int             STDCALL myblockchain_get_option(MYBLOCKCHAIN *myblockchain, enum myblockchain_option option,
                                         const void *arg);
void		STDCALL myblockchain_free_result(MYBLOCKCHAIN_RES *result);
void		STDCALL myblockchain_data_seek(MYBLOCKCHAIN_RES *result,
					my_ulonglong offset);
MYBLOCKCHAIN_ROW_OFFSET STDCALL myblockchain_row_seek(MYBLOCKCHAIN_RES *result,
						MYBLOCKCHAIN_ROW_OFFSET offset);
MYBLOCKCHAIN_FIELD_OFFSET STDCALL myblockchain_field_seek(MYBLOCKCHAIN_RES *result,
					   MYBLOCKCHAIN_FIELD_OFFSET offset);
MYBLOCKCHAIN_ROW	STDCALL myblockchain_fetch_row(MYBLOCKCHAIN_RES *result);
unsigned long * STDCALL myblockchain_fetch_lengths(MYBLOCKCHAIN_RES *result);
MYBLOCKCHAIN_FIELD *	STDCALL myblockchain_fetch_field(MYBLOCKCHAIN_RES *result);
MYBLOCKCHAIN_RES *     STDCALL myblockchain_list_fields(MYBLOCKCHAIN *myblockchain, const char *table,
					  const char *wild);
unsigned long	STDCALL myblockchain_escape_string(char *to,const char *from,
					    unsigned long from_length);
unsigned long	STDCALL myblockchain_hex_string(char *to,const char *from,
                                         unsigned long from_length);
unsigned long STDCALL myblockchain_real_escape_string(MYBLOCKCHAIN *myblockchain,
					       char *to,const char *from,
					       unsigned long length);
unsigned long STDCALL myblockchain_real_escape_string_quote(MYBLOCKCHAIN *myblockchain,
                 char *to, const char *from,
                 unsigned long length, char quote);
void          STDCALL myblockchain_debug(const char *debug);
void          STDCALL myodbc_remove_escape(MYBLOCKCHAIN *myblockchain,char *name);
unsigned int  STDCALL myblockchain_thread_safe(void);
my_bool       STDCALL myblockchain_embedded(void);
my_bool       STDCALL myblockchain_read_query_result(MYBLOCKCHAIN *myblockchain);
int           STDCALL myblockchain_reset_connection(MYBLOCKCHAIN *myblockchain);

/*
  The following definitions are added for the enhanced 
  client-server protocol
*/

/* statement state */
enum enum_myblockchain_stmt_state
{
  MYBLOCKCHAIN_STMT_INIT_DONE= 1, MYBLOCKCHAIN_STMT_PREPARE_DONE, MYBLOCKCHAIN_STMT_EXECUTE_DONE,
  MYBLOCKCHAIN_STMT_FETCH_DONE
};


/*
  This structure is used to define bind information, and
  internally by the client library.
  Public members with their descriptions are listed below
  (conventionally `On input' refers to the binds given to
  myblockchain_stmt_bind_param, `On output' refers to the binds given
  to myblockchain_stmt_bind_result):

  buffer_type    - One of the MYBLOCKCHAIN_* types, used to describe
                   the host language type of buffer.
                   On output: if column type is different from
                   buffer_type, column value is automatically converted
                   to buffer_type before it is stored in the buffer.
  buffer         - On input: points to the buffer with input data.
                   On output: points to the buffer capable to store
                   output data.
                   The type of memory pointed by buffer must correspond
                   to buffer_type. See the correspondence table in
                   the comment to myblockchain_stmt_bind_param.

  The two above members are mandatory for any kind of bind.

  buffer_length  - the length of the buffer. You don't have to set
                   it for any fixed length buffer: float, double,
                   int, etc. It must be set however for variable-length
                   types, such as BLOBs or STRINGs.

  length         - On input: in case when lengths of input values
                   are different for each execute, you can set this to
                   point at a variable containining value length. This
                   way the value length can be different in each execute.
                   If length is not NULL, buffer_length is not used.
                   Note, length can even point at buffer_length if
                   you keep bind structures around while fetching:
                   this way you can change buffer_length before
                   each execution, everything will work ok.
                   On output: if length is set, myblockchain_stmt_fetch will
                   write column length into it.

  is_null        - On input: points to a boolean variable that should
                   be set to TRUE for NULL values.
                   This member is useful only if your data may be
                   NULL in some but not all cases.
                   If your data is never NULL, is_null should be set to 0.
                   If your data is always NULL, set buffer_type
                   to MYBLOCKCHAIN_TYPE_NULL, and is_null will not be used.

  is_unsigned    - On input: used to signify that values provided for one
                   of numeric types are unsigned.
                   On output describes signedness of the output buffer.
                   If, taking into account is_unsigned flag, column data
                   is out of range of the output buffer, data for this column
                   is regarded truncated. Note that this has no correspondence
                   to the sign of result set column, if you need to find it out
                   use myblockchain_stmt_result_metadata.
  error          - where to write a truncation error if it is present.
                   possible error value is:
                   0  no truncation
                   1  value is out of range or buffer is too small

  Please note that MYBLOCKCHAIN_BIND also has internals members.
*/

typedef struct st_myblockchain_bind
{
  unsigned long	*length;          /* output length pointer */
  my_bool       *is_null;	  /* Pointer to null indicator */
  void		*buffer;	  /* buffer to get/put data */
  /* set this if you want to track data truncations happened during fetch */
  my_bool       *error;
  unsigned char *row_ptr;         /* for the current data position */
  void (*store_param_func)(NET *net, struct st_myblockchain_bind *param);
  void (*fetch_result)(struct st_myblockchain_bind *, MYBLOCKCHAIN_FIELD *,
                       unsigned char **row);
  void (*skip_result)(struct st_myblockchain_bind *, MYBLOCKCHAIN_FIELD *,
		      unsigned char **row);
  /* output buffer length, must be set when fetching str/binary */
  unsigned long buffer_length;
  unsigned long offset;           /* offset position for char/binary fetch */
  unsigned long length_value;     /* Used if length is 0 */
  unsigned int	param_number;	  /* For null count and error messages */
  unsigned int  pack_length;	  /* Internal length for packed data */
  enum enum_field_types buffer_type;	/* buffer type */
  my_bool       error_value;      /* used if error is 0 */
  my_bool       is_unsigned;      /* set if integer type is unsigned */
  my_bool	long_data_used;	  /* If used with myblockchain_send_long_data */
  my_bool	is_null_value;    /* Used if is_null is 0 */
  void *extension;
} MYBLOCKCHAIN_BIND;


struct st_myblockchain_stmt_extension;

/* statement handler */
typedef struct st_myblockchain_stmt
{
  MEM_ROOT       mem_root;             /* root allocations */
  LIST           list;                 /* list to keep track of all stmts */
  MYBLOCKCHAIN          *myblockchain;               /* connection handle */
  MYBLOCKCHAIN_BIND     *params;              /* input parameters */
  MYBLOCKCHAIN_BIND     *bind;                /* output parameters */
  MYBLOCKCHAIN_FIELD    *fields;              /* result set metadata */
  MYBLOCKCHAIN_DATA     result;               /* cached result set */
  MYBLOCKCHAIN_ROWS     *data_cursor;         /* current row in cached result */
  /*
    myblockchain_stmt_fetch() calls this function to fetch one row (it's different
    for buffered, unbuffered and cursor fetch).
  */
  int            (*read_row_func)(struct st_myblockchain_stmt *stmt, 
                                  unsigned char **row);
  /* copy of myblockchain->affected_rows after statement execution */
  my_ulonglong   affected_rows;
  my_ulonglong   insert_id;            /* copy of myblockchain->insert_id */
  unsigned long	 stmt_id;	       /* Id for prepared statement */
  unsigned long  flags;                /* i.e. type of cursor to open */
  unsigned long  prefetch_rows;        /* number of rows per one COM_FETCH */
  /*
    Copied from myblockchain->server_status after execute/fetch to know
    server-side cursor status for this statement.
  */
  unsigned int   server_status;
  unsigned int	 last_errno;	       /* error code */
  unsigned int   param_count;          /* input parameter count */
  unsigned int   field_count;          /* number of columns in result set */
  enum enum_myblockchain_stmt_state state;    /* statement state */
  char		 last_error[MYBLOCKCHAIN_ERRMSG_SIZE]; /* error message */
  char		 sqlstate[SQLSTATE_LENGTH+1];
  /* Types of input parameters should be sent to server */
  my_bool        send_types_to_server;
  my_bool        bind_param_done;      /* input buffers were supplied */
  unsigned char  bind_result_done;     /* output buffers were supplied */
  /* myblockchain_stmt_close() had to cancel this result */
  my_bool       unbuffered_fetch_cancelled;  
  /*
    Is set to true if we need to calculate field->max_length for 
    metadata fields when doing myblockchain_stmt_store_result.
  */
  my_bool       update_max_length;     
  struct st_myblockchain_stmt_extension *extension;
} MYBLOCKCHAIN_STMT;

enum enum_stmt_attr_type
{
  /*
    When doing myblockchain_stmt_store_result calculate max_length attribute
    of statement metadata. This is to be consistent with the old API, 
    where this was done automatically.
    In the new API we do that only by request because it slows down
    myblockchain_stmt_store_result sufficiently.
  */
  STMT_ATTR_UPDATE_MAX_LENGTH,
  /*
    unsigned long with combination of cursor flags (read only, for update,
    etc)
  */
  STMT_ATTR_CURSOR_TYPE,
  /*
    Amount of rows to retrieve from server per one fetch if using cursors.
    Accepts unsigned long attribute in the range 1 - ulong_max
  */
  STMT_ATTR_PREFETCH_ROWS
};


MYBLOCKCHAIN_STMT * STDCALL myblockchain_stmt_init(MYBLOCKCHAIN *myblockchain);
int STDCALL myblockchain_stmt_prepare(MYBLOCKCHAIN_STMT *stmt, const char *query,
                               unsigned long length);
int STDCALL myblockchain_stmt_execute(MYBLOCKCHAIN_STMT *stmt);
int STDCALL myblockchain_stmt_fetch(MYBLOCKCHAIN_STMT *stmt);
int STDCALL myblockchain_stmt_fetch_column(MYBLOCKCHAIN_STMT *stmt, MYBLOCKCHAIN_BIND *bind_arg, 
                                    unsigned int column,
                                    unsigned long offset);
int STDCALL myblockchain_stmt_store_result(MYBLOCKCHAIN_STMT *stmt);
unsigned long STDCALL myblockchain_stmt_param_count(MYBLOCKCHAIN_STMT * stmt);
my_bool STDCALL myblockchain_stmt_attr_set(MYBLOCKCHAIN_STMT *stmt,
                                    enum enum_stmt_attr_type attr_type,
                                    const void *attr);
my_bool STDCALL myblockchain_stmt_attr_get(MYBLOCKCHAIN_STMT *stmt,
                                    enum enum_stmt_attr_type attr_type,
                                    void *attr);
my_bool STDCALL myblockchain_stmt_bind_param(MYBLOCKCHAIN_STMT * stmt, MYBLOCKCHAIN_BIND * bnd);
my_bool STDCALL myblockchain_stmt_bind_result(MYBLOCKCHAIN_STMT * stmt, MYBLOCKCHAIN_BIND * bnd);
my_bool STDCALL myblockchain_stmt_close(MYBLOCKCHAIN_STMT * stmt);
my_bool STDCALL myblockchain_stmt_reset(MYBLOCKCHAIN_STMT * stmt);
my_bool STDCALL myblockchain_stmt_free_result(MYBLOCKCHAIN_STMT *stmt);
my_bool STDCALL myblockchain_stmt_send_long_data(MYBLOCKCHAIN_STMT *stmt, 
                                          unsigned int param_number,
                                          const char *data, 
                                          unsigned long length);
MYBLOCKCHAIN_RES *STDCALL myblockchain_stmt_result_metadata(MYBLOCKCHAIN_STMT *stmt);
MYBLOCKCHAIN_RES *STDCALL myblockchain_stmt_param_metadata(MYBLOCKCHAIN_STMT *stmt);
unsigned int STDCALL myblockchain_stmt_errno(MYBLOCKCHAIN_STMT * stmt);
const char *STDCALL myblockchain_stmt_error(MYBLOCKCHAIN_STMT * stmt);
const char *STDCALL myblockchain_stmt_sqlstate(MYBLOCKCHAIN_STMT * stmt);
MYBLOCKCHAIN_ROW_OFFSET STDCALL myblockchain_stmt_row_seek(MYBLOCKCHAIN_STMT *stmt, 
                                             MYBLOCKCHAIN_ROW_OFFSET offset);
MYBLOCKCHAIN_ROW_OFFSET STDCALL myblockchain_stmt_row_tell(MYBLOCKCHAIN_STMT *stmt);
void STDCALL myblockchain_stmt_data_seek(MYBLOCKCHAIN_STMT *stmt, my_ulonglong offset);
my_ulonglong STDCALL myblockchain_stmt_num_rows(MYBLOCKCHAIN_STMT *stmt);
my_ulonglong STDCALL myblockchain_stmt_affected_rows(MYBLOCKCHAIN_STMT *stmt);
my_ulonglong STDCALL myblockchain_stmt_insert_id(MYBLOCKCHAIN_STMT *stmt);
unsigned int STDCALL myblockchain_stmt_field_count(MYBLOCKCHAIN_STMT *stmt);

my_bool STDCALL myblockchain_commit(MYBLOCKCHAIN * myblockchain);
my_bool STDCALL myblockchain_rollback(MYBLOCKCHAIN * myblockchain);
my_bool STDCALL myblockchain_autocommit(MYBLOCKCHAIN * myblockchain, my_bool auto_mode);
my_bool STDCALL myblockchain_more_results(MYBLOCKCHAIN *myblockchain);
int STDCALL myblockchain_next_result(MYBLOCKCHAIN *myblockchain);
int STDCALL myblockchain_stmt_next_result(MYBLOCKCHAIN_STMT *stmt);
void STDCALL myblockchain_close(MYBLOCKCHAIN *sock);


/* status return codes */
#define MYBLOCKCHAIN_NO_DATA        100
#define MYBLOCKCHAIN_DATA_TRUNCATED 101

#define myblockchain_reload(myblockchain) myblockchain_refresh((myblockchain),REFRESH_GRANT)

#define HAVE_MYBLOCKCHAIN_REAL_CONNECT

#ifdef	__cplusplus
}
#endif

#endif /* _myblockchain_h */
