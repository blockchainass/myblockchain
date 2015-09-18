typedef char my_bool;
typedef int my_socket;
#include "myblockchain_version.h"
#include "myblockchain_com.h"
#include "binary_log_types.h"
typedef enum enum_field_types {
  MYBLOCKCHAIN_TYPE_DECIMAL, MYBLOCKCHAIN_TYPE_TINY,
  MYBLOCKCHAIN_TYPE_SHORT, MYBLOCKCHAIN_TYPE_LONG,
  MYBLOCKCHAIN_TYPE_FLOAT, MYBLOCKCHAIN_TYPE_DOUBLE,
  MYBLOCKCHAIN_TYPE_NULL, MYBLOCKCHAIN_TYPE_TIMESTAMP,
  MYBLOCKCHAIN_TYPE_LONGLONG,MYBLOCKCHAIN_TYPE_INT24,
  MYBLOCKCHAIN_TYPE_DATE, MYBLOCKCHAIN_TYPE_TIME,
  MYBLOCKCHAIN_TYPE_DATETIME, MYBLOCKCHAIN_TYPE_YEAR,
  MYBLOCKCHAIN_TYPE_NEWDATE, MYBLOCKCHAIN_TYPE_VARCHAR,
  MYBLOCKCHAIN_TYPE_BIT,
  MYBLOCKCHAIN_TYPE_TIMESTAMP2,
  MYBLOCKCHAIN_TYPE_DATETIME2,
  MYBLOCKCHAIN_TYPE_TIME2,
  MYBLOCKCHAIN_TYPE_JSON=245,
  MYBLOCKCHAIN_TYPE_NEWDECIMAL=246,
  MYBLOCKCHAIN_TYPE_ENUM=247,
  MYBLOCKCHAIN_TYPE_SET=248,
  MYBLOCKCHAIN_TYPE_TINY_BLOB=249,
  MYBLOCKCHAIN_TYPE_MEDIUM_BLOB=250,
  MYBLOCKCHAIN_TYPE_LONG_BLOB=251,
  MYBLOCKCHAIN_TYPE_BLOB=252,
  MYBLOCKCHAIN_TYPE_VAR_STRING=253,
  MYBLOCKCHAIN_TYPE_STRING=254,
  MYBLOCKCHAIN_TYPE_GEOMETRY=255
} enum_field_types;
enum enum_server_command
{
  COM_SLEEP, COM_QUIT, COM_INIT_DB, COM_QUERY, COM_FIELD_LIST,
  COM_CREATE_DB, COM_DROP_DB, COM_REFRESH, COM_SHUTDOWN, COM_STATISTICS,
  COM_PROCESS_INFO, COM_CONNECT, COM_PROCESS_KILL, COM_DEBUG, COM_PING,
  COM_TIME, COM_DELAYED_INSERT, COM_CHANGE_USER, COM_BINLOG_DUMP,
  COM_TABLE_DUMP, COM_CONNECT_OUT, COM_REGISTER_SLAVE,
  COM_STMT_PREPARE, COM_STMT_EXECUTE, COM_STMT_SEND_LONG_DATA, COM_STMT_CLOSE,
  COM_STMT_RESET, COM_SET_OPTION, COM_STMT_FETCH, COM_DAEMON,
  COM_BINLOG_DUMP_GTID, COM_RESET_CONNECTION,
  COM_END
};
struct st_vio;
typedef struct st_vio Vio;
typedef struct st_net {
  Vio *vio;
  unsigned char *buff,*buff_end,*write_pos,*read_pos;
  my_socket fd;
  unsigned long remain_in_buf,length, buf_length, where_b;
  unsigned long max_packet,max_packet_size;
  unsigned int pkt_nr,compress_pkt_nr;
  unsigned int write_timeout, read_timeout, retry_count;
  int fcntl;
  unsigned int *return_status;
  unsigned char reading_or_writing;
  char save_char;
  my_bool unused1;
  my_bool unused2;
  my_bool compress;
  my_bool unused3;
  unsigned char *unused;
  unsigned int last_errno;
  unsigned char error;
  my_bool unused4;
  my_bool unused5;
  char last_error[512];
  char sqlstate[5 +1];
  void *extension;
} NET;
enum myblockchain_enum_shutdown_level {
  SHUTDOWN_DEFAULT = 0,
  SHUTDOWN_WAIT_CONNECTIONS= (unsigned char)(1 << 0),
  SHUTDOWN_WAIT_TRANSACTIONS= (unsigned char)(1 << 1),
  SHUTDOWN_WAIT_UPDATES= (unsigned char)(1 << 3),
  SHUTDOWN_WAIT_ALL_BUFFERS= ((unsigned char)(1 << 3) << 1),
  SHUTDOWN_WAIT_CRITICAL_BUFFERS= ((unsigned char)(1 << 3) << 1) + 1,
  KILL_QUERY= 254,
  KILL_CONNECTION= 255
};
enum enum_cursor_type
{
  CURSOR_TYPE_NO_CURSOR= 0,
  CURSOR_TYPE_READ_ONLY= 1,
  CURSOR_TYPE_FOR_UPDATE= 2,
  CURSOR_TYPE_SCROLLABLE= 4
};
enum enum_myblockchain_set_option
{
  MYBLOCKCHAIN_OPTION_MULTI_STATEMENTS_ON,
  MYBLOCKCHAIN_OPTION_MULTI_STATEMENTS_OFF
};
enum enum_session_state_type
{
  SESSION_TRACK_SYSTEM_VARIABLES,
  SESSION_TRACK_SCHEMA,
  SESSION_TRACK_STATE_CHANGE,
  SESSION_TRACK_GTIDS,
  SESSION_TRACK_TRANSACTION_CHARACTERISTICS,
  SESSION_TRACK_TRANSACTION_STATE
};
my_bool my_net_init(NET *net, Vio* vio);
void my_net_local_init(NET *net);
void net_end(NET *net);
void net_clear(NET *net, my_bool check_buffer);
void net_claim_memory_ownership(NET *net);
my_bool net_realloc(NET *net, size_t length);
my_bool net_flush(NET *net);
my_bool my_net_write(NET *net,const unsigned char *packet, size_t len);
my_bool net_write_command(NET *net,unsigned char command,
     const unsigned char *header, size_t head_len,
     const unsigned char *packet, size_t len);
my_bool net_write_packet(NET *net, const unsigned char *packet, size_t length);
unsigned long my_net_read(NET *net);
struct rand_struct {
  unsigned long seed1,seed2,max_value;
  double max_value_dbl;
};
enum Item_result {STRING_RESULT=0, REAL_RESULT, INT_RESULT, ROW_RESULT,
                  DECIMAL_RESULT};
typedef struct st_udf_args
{
  unsigned int arg_count;
  enum Item_result *arg_type;
  char **args;
  unsigned long *lengths;
  char *maybe_null;
  char **attributes;
  unsigned long *attribute_lengths;
  void *extension;
} UDF_ARGS;
typedef struct st_udf_init
{
  my_bool maybe_null;
  unsigned int decimals;
  unsigned long max_length;
  char *ptr;
  my_bool const_item;
  void *extension;
} UDF_INIT;
void randominit(struct rand_struct *, unsigned long seed1,
                unsigned long seed2);
double my_rnd(struct rand_struct *);
void create_random_string(char *to, unsigned int length, struct rand_struct *rand_st);
void hash_password(unsigned long *to, const char *password, unsigned int password_len);
void make_scrambled_password_323(char *to, const char *password);
void scramble_323(char *to, const char *message, const char *password);
my_bool check_scramble_323(const unsigned char *reply, const char *message,
                           unsigned long *salt);
void get_salt_from_password_323(unsigned long *res, const char *password);
void make_password_from_salt_323(char *to, const unsigned long *salt);
void make_scrambled_password(char *to, const char *password);
void scramble(char *to, const char *message, const char *password);
my_bool check_scramble(const unsigned char *reply, const char *message,
                       const unsigned char *hash_stage2);
void get_salt_from_password(unsigned char *res, const char *password);
void make_password_from_salt(char *to, const unsigned char *hash_stage2);
char *octet2hex(char *to, const char *str, unsigned int len);
char *get_tty_password(const char *opt_message);
const char *myblockchain_errno_to_sqlstate(unsigned int myblockchain_errno);
my_bool my_thread_init(void);
void my_thread_end(void);
#include "myblockchain_time.h"
enum enum_myblockchain_timestamp_type
{
  MYBLOCKCHAIN_TIMESTAMP_NONE= -2, MYBLOCKCHAIN_TIMESTAMP_ERROR= -1,
  MYBLOCKCHAIN_TIMESTAMP_DATE= 0, MYBLOCKCHAIN_TIMESTAMP_DATETIME= 1, MYBLOCKCHAIN_TIMESTAMP_TIME= 2
};
typedef struct st_myblockchain_time
{
  unsigned int year, month, day, hour, minute, second;
  unsigned long second_part;
  my_bool neg;
  enum enum_myblockchain_timestamp_type time_type;
} MYBLOCKCHAIN_TIME;
#include "my_list.h"
typedef struct st_list {
  struct st_list *prev,*next;
  void *data;
} LIST;
typedef int (*list_walk_action)(void *,void *);
extern LIST *list_add(LIST *root,LIST *element);
extern LIST *list_delete(LIST *root,LIST *element);
extern LIST *list_cons(void *data,LIST *root);
extern LIST *list_reverse(LIST *root);
extern void list_free(LIST *root,unsigned int free_data);
extern unsigned int list_length(LIST *);
extern int list_walk(LIST *,list_walk_action action,unsigned char * argument);
#include "myblockchain/client_plugin.h"
struct st_myblockchain_client_plugin
{
  int type; unsigned int interface_version; const char *name; const char *author; const char *desc; unsigned int version[3]; const char *license; void *myblockchain_api; int (*init)(char *, size_t, int, va_list); int (*deinit)(); int (*options)(const char *option, const void *);
};
struct st_myblockchain;
#include <myblockchain/plugin_auth_common.h>
typedef struct st_plugin_vio_info
{
  enum { MYBLOCKCHAIN_VIO_INVALID, MYBLOCKCHAIN_VIO_TCP, MYBLOCKCHAIN_VIO_SOCKET,
         MYBLOCKCHAIN_VIO_PIPE, MYBLOCKCHAIN_VIO_MEMORY } protocol;
  int socket;
} MYBLOCKCHAIN_PLUGIN_VIO_INFO;
typedef struct st_plugin_vio
{
  int (*read_packet)(struct st_plugin_vio *vio,
                     unsigned char **buf);
  int (*write_packet)(struct st_plugin_vio *vio,
                      const unsigned char *packet,
                      int packet_len);
  void (*info)(struct st_plugin_vio *vio, struct st_plugin_vio_info *info);
} MYBLOCKCHAIN_PLUGIN_VIO;
struct st_myblockchain_client_plugin_AUTHENTICATION
{
  int type; unsigned int interface_version; const char *name; const char *author; const char *desc; unsigned int version[3]; const char *license; void *myblockchain_api; int (*init)(char *, size_t, int, va_list); int (*deinit)(); int (*options)(const char *option, const void *);
  int (*authenticate_user)(MYBLOCKCHAIN_PLUGIN_VIO *vio, struct st_myblockchain *myblockchain);
};
struct st_myblockchain_client_plugin *
myblockchain_load_plugin(struct st_myblockchain *myblockchain, const char *name, int type,
                  int argc, ...);
struct st_myblockchain_client_plugin *
myblockchain_load_plugin_v(struct st_myblockchain *myblockchain, const char *name, int type,
                    int argc, va_list args);
struct st_myblockchain_client_plugin *
myblockchain_client_find_plugin(struct st_myblockchain *myblockchain, const char *name, int type);
struct st_myblockchain_client_plugin *
myblockchain_client_register_plugin(struct st_myblockchain *myblockchain,
                             struct st_myblockchain_client_plugin *plugin);
int myblockchain_plugin_options(struct st_myblockchain_client_plugin *plugin,
                         const char *option, const void *value);
extern unsigned int myblockchain_port;
extern char *myblockchain_unix_port;
typedef struct st_myblockchain_field {
  char *name;
  char *org_name;
  char *table;
  char *org_table;
  char *db;
  char *catalog;
  char *def;
  unsigned long length;
  unsigned long max_length;
  unsigned int name_length;
  unsigned int org_name_length;
  unsigned int table_length;
  unsigned int org_table_length;
  unsigned int db_length;
  unsigned int catalog_length;
  unsigned int def_length;
  unsigned int flags;
  unsigned int decimals;
  unsigned int charsetnr;
  enum enum_field_types type;
  void *extension;
} MYBLOCKCHAIN_FIELD;
typedef char **MYBLOCKCHAIN_ROW;
typedef unsigned int MYBLOCKCHAIN_FIELD_OFFSET;
typedef unsigned long long my_ulonglong;
#include "typelib.h"
#include "my_alloc.h"
#include <myblockchain/psi/psi_memory.h>
#include "psi_base.h"
struct PSI_thread;
typedef unsigned int PSI_memory_key;
typedef struct st_used_mem
{
  struct st_used_mem *next;
  unsigned int left;
  unsigned int size;
} USED_MEM;
typedef struct st_mem_root
{
  USED_MEM *free;
  USED_MEM *used;
  USED_MEM *pre_alloc;
  size_t min_malloc;
  size_t block_size;
  unsigned int block_num;
  unsigned int first_block_usage;
  void (*error_handler)(void);
  PSI_memory_key m_psi_key;
} MEM_ROOT;
typedef struct st_typelib {
  unsigned int count;
  const char *name;
  const char **type_names;
  unsigned int *type_lengths;
} TYPELIB;
extern my_ulonglong find_typeset(char *x, TYPELIB *typelib,int *error_position);
extern int find_type_or_exit(const char *x, TYPELIB *typelib,
                             const char *option);
extern int find_type(const char *x, const TYPELIB *typelib, unsigned int flags);
extern void make_type(char *to,unsigned int nr,TYPELIB *typelib);
extern const char *get_type(TYPELIB *typelib,unsigned int nr);
extern TYPELIB *copy_typelib(MEM_ROOT *root, TYPELIB *from);
extern TYPELIB sql_protocol_typelib;
my_ulonglong find_set_from_flags(const TYPELIB *lib, unsigned int default_name,
                              my_ulonglong cur_set, my_ulonglong default_set,
                              const char *str, unsigned int length,
                              char **err_pos, unsigned int *err_len);
typedef struct st_myblockchain_rows {
  struct st_myblockchain_rows *next;
  MYBLOCKCHAIN_ROW data;
  unsigned long length;
} MYBLOCKCHAIN_ROWS;
typedef MYBLOCKCHAIN_ROWS *MYBLOCKCHAIN_ROW_OFFSET;
#include "my_alloc.h"
typedef struct embedded_query_result EMBEDDED_QUERY_RESULT;
typedef struct st_myblockchain_data {
  MYBLOCKCHAIN_ROWS *data;
  struct embedded_query_result *embedded_info;
  MEM_ROOT alloc;
  my_ulonglong rows;
  unsigned int fields;
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
struct st_myblockchain_options_extention;
struct st_myblockchain_options {
  unsigned int connect_timeout, read_timeout, write_timeout;
  unsigned int port, protocol;
  unsigned long client_flag;
  char *host,*user,*password,*unix_socket,*db;
  struct st_dynamic_array *init_commands;
  char *my_cnf_file,*my_cnf_group, *charset_dir, *charset_name;
  char *ssl_key;
  char *ssl_cert;
  char *ssl_ca;
  char *ssl_capath;
  char *ssl_cipher;
  char *shared_memory_base_name;
  unsigned long max_allowed_packet;
  my_bool use_ssl;
  my_bool compress,named_pipe;
  my_bool unused1;
  my_bool unused2;
  my_bool unused3;
  my_bool unused4;
  enum myblockchain_option methods_to_use;
  union {
    char *client_ip;
    char *bind_address;
  } ci;
  my_bool unused5;
  my_bool report_data_truncation;
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
  unsigned int number;
  unsigned int state;
  const char *csname;
  const char *name;
  const char *comment;
  const char *dir;
  unsigned int mbminlen;
  unsigned int mbmaxlen;
} MY_CHARSET_INFO;
struct st_myblockchain_methods;
struct st_myblockchain_stmt;
typedef struct st_myblockchain
{
  NET net;
  unsigned char *connector_fd;
  char *host,*user,*passwd,*unix_socket,*server_version,*host_info;
  char *info, *db;
  struct charset_info_st *charset;
  MYBLOCKCHAIN_FIELD *fields;
  MEM_ROOT field_alloc;
  my_ulonglong affected_rows;
  my_ulonglong insert_id;
  my_ulonglong extra_info;
  unsigned long thread_id;
  unsigned long packet_length;
  unsigned int port;
  unsigned long client_flag,server_capabilities;
  unsigned int protocol_version;
  unsigned int field_count;
  unsigned int server_status;
  unsigned int server_language;
  unsigned int warning_count;
  struct st_myblockchain_options options;
  enum myblockchain_status status;
  my_bool free_me;
  my_bool reconnect;
  char scramble[20 +1];
  my_bool unused1;
  void *unused2, *unused3, *unused4, *unused5;
  LIST *stmts;
  const struct st_myblockchain_methods *methods;
  void *thd;
  my_bool *unbuffered_fetch_owner;
  char *info_buffer;
  void *extension;
} MYBLOCKCHAIN;
typedef struct st_myblockchain_res {
  my_ulonglong row_count;
  MYBLOCKCHAIN_FIELD *fields;
  MYBLOCKCHAIN_DATA *data;
  MYBLOCKCHAIN_ROWS *data_cursor;
  unsigned long *lengths;
  MYBLOCKCHAIN *handle;
  const struct st_myblockchain_methods *methods;
  MYBLOCKCHAIN_ROW row;
  MYBLOCKCHAIN_ROW current_row;
  MEM_ROOT field_alloc;
  unsigned int field_count, current_field;
  my_bool eof;
  my_bool unbuffered_fetch_cancelled;
  void *extension;
} MYBLOCKCHAIN_RES;
typedef struct st_myblockchain_parameters
{
  unsigned long *p_max_allowed_packet;
  unsigned long *p_net_buffer_length;
  void *extension;
} MYBLOCKCHAIN_PARAMETERS;
int myblockchain_server_init(int argc, char **argv, char **groups);
void myblockchain_server_end(void);
MYBLOCKCHAIN_PARAMETERS * myblockchain_get_parameters(void);
my_bool myblockchain_thread_init(void);
void myblockchain_thread_end(void);
my_ulonglong myblockchain_num_rows(MYBLOCKCHAIN_RES *res);
unsigned int myblockchain_num_fields(MYBLOCKCHAIN_RES *res);
my_bool myblockchain_eof(MYBLOCKCHAIN_RES *res);
MYBLOCKCHAIN_FIELD * myblockchain_fetch_field_direct(MYBLOCKCHAIN_RES *res,
           unsigned int fieldnr);
MYBLOCKCHAIN_FIELD * myblockchain_fetch_fields(MYBLOCKCHAIN_RES *res);
MYBLOCKCHAIN_ROW_OFFSET myblockchain_row_tell(MYBLOCKCHAIN_RES *res);
MYBLOCKCHAIN_FIELD_OFFSET myblockchain_field_tell(MYBLOCKCHAIN_RES *res);
unsigned int myblockchain_field_count(MYBLOCKCHAIN *myblockchain);
my_ulonglong myblockchain_affected_rows(MYBLOCKCHAIN *myblockchain);
my_ulonglong myblockchain_insert_id(MYBLOCKCHAIN *myblockchain);
unsigned int myblockchain_errno(MYBLOCKCHAIN *myblockchain);
const char * myblockchain_error(MYBLOCKCHAIN *myblockchain);
const char * myblockchain_sqlstate(MYBLOCKCHAIN *myblockchain);
unsigned int myblockchain_warning_count(MYBLOCKCHAIN *myblockchain);
const char * myblockchain_info(MYBLOCKCHAIN *myblockchain);
unsigned long myblockchain_thread_id(MYBLOCKCHAIN *myblockchain);
const char * myblockchain_character_set_name(MYBLOCKCHAIN *myblockchain);
int myblockchain_set_character_set(MYBLOCKCHAIN *myblockchain, const char *csname);
MYBLOCKCHAIN * myblockchain_init(MYBLOCKCHAIN *myblockchain);
my_bool myblockchain_ssl_set(MYBLOCKCHAIN *myblockchain, const char *key,
          const char *cert, const char *ca,
          const char *capath, const char *cipher);
const char * myblockchain_get_ssl_cipher(MYBLOCKCHAIN *myblockchain);
my_bool myblockchain_change_user(MYBLOCKCHAIN *myblockchain, const char *user,
       const char *passwd, const char *db);
MYBLOCKCHAIN * myblockchain_real_connect(MYBLOCKCHAIN *myblockchain, const char *host,
        const char *user,
        const char *passwd,
        const char *db,
        unsigned int port,
        const char *unix_socket,
        unsigned long clientflag);
int myblockchain_select_db(MYBLOCKCHAIN *myblockchain, const char *db);
int myblockchain_query(MYBLOCKCHAIN *myblockchain, const char *q);
int myblockchain_send_query(MYBLOCKCHAIN *myblockchain, const char *q,
      unsigned long length);
int myblockchain_real_query(MYBLOCKCHAIN *myblockchain, const char *q,
     unsigned long length);
MYBLOCKCHAIN_RES * myblockchain_store_result(MYBLOCKCHAIN *myblockchain);
MYBLOCKCHAIN_RES * myblockchain_use_result(MYBLOCKCHAIN *myblockchain);
void myblockchain_get_character_set_info(MYBLOCKCHAIN *myblockchain,
                           MY_CHARSET_INFO *charset);
int myblockchain_session_track_get_first(MYBLOCKCHAIN *myblockchain,
                                          enum enum_session_state_type type,
                                          const char **data,
                                          size_t *length);
int myblockchain_session_track_get_next(MYBLOCKCHAIN *myblockchain,
                                         enum enum_session_state_type type,
                                         const char **data,
                                         size_t *length);
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
int myblockchain_shutdown(MYBLOCKCHAIN *myblockchain,
                                       enum myblockchain_enum_shutdown_level
                                       shutdown_level);
int myblockchain_dump_debug_info(MYBLOCKCHAIN *myblockchain);
int myblockchain_refresh(MYBLOCKCHAIN *myblockchain,
         unsigned int refresh_options);
int myblockchain_kill(MYBLOCKCHAIN *myblockchain,unsigned long pid);
int myblockchain_set_server_option(MYBLOCKCHAIN *myblockchain,
      enum enum_myblockchain_set_option
      option);
int myblockchain_ping(MYBLOCKCHAIN *myblockchain);
const char * myblockchain_stat(MYBLOCKCHAIN *myblockchain);
const char * myblockchain_get_server_info(MYBLOCKCHAIN *myblockchain);
const char * myblockchain_get_client_info(void);
unsigned long myblockchain_get_client_version(void);
const char * myblockchain_get_host_info(MYBLOCKCHAIN *myblockchain);
unsigned long myblockchain_get_server_version(MYBLOCKCHAIN *myblockchain);
unsigned int myblockchain_get_proto_info(MYBLOCKCHAIN *myblockchain);
MYBLOCKCHAIN_RES * myblockchain_list_dbs(MYBLOCKCHAIN *myblockchain,const char *wild);
MYBLOCKCHAIN_RES * myblockchain_list_tables(MYBLOCKCHAIN *myblockchain,const char *wild);
MYBLOCKCHAIN_RES * myblockchain_list_processes(MYBLOCKCHAIN *myblockchain);
int myblockchain_options(MYBLOCKCHAIN *myblockchain,enum myblockchain_option option,
          const void *arg);
int myblockchain_options4(MYBLOCKCHAIN *myblockchain,enum myblockchain_option option,
                                       const void *arg1, const void *arg2);
int myblockchain_get_option(MYBLOCKCHAIN *myblockchain, enum myblockchain_option option,
                                         const void *arg);
void myblockchain_free_result(MYBLOCKCHAIN_RES *result);
void myblockchain_data_seek(MYBLOCKCHAIN_RES *result,
     my_ulonglong offset);
MYBLOCKCHAIN_ROW_OFFSET myblockchain_row_seek(MYBLOCKCHAIN_RES *result,
      MYBLOCKCHAIN_ROW_OFFSET offset);
MYBLOCKCHAIN_FIELD_OFFSET myblockchain_field_seek(MYBLOCKCHAIN_RES *result,
        MYBLOCKCHAIN_FIELD_OFFSET offset);
MYBLOCKCHAIN_ROW myblockchain_fetch_row(MYBLOCKCHAIN_RES *result);
unsigned long * myblockchain_fetch_lengths(MYBLOCKCHAIN_RES *result);
MYBLOCKCHAIN_FIELD * myblockchain_fetch_field(MYBLOCKCHAIN_RES *result);
MYBLOCKCHAIN_RES * myblockchain_list_fields(MYBLOCKCHAIN *myblockchain, const char *table,
       const char *wild);
unsigned long myblockchain_escape_string(char *to,const char *from,
         unsigned long from_length);
unsigned long myblockchain_hex_string(char *to,const char *from,
                                         unsigned long from_length);
unsigned long myblockchain_real_escape_string(MYBLOCKCHAIN *myblockchain,
            char *to,const char *from,
            unsigned long length);
unsigned long myblockchain_real_escape_string_quote(MYBLOCKCHAIN *myblockchain,
                 char *to, const char *from,
                 unsigned long length, char quote);
void myblockchain_debug(const char *debug);
void myodbc_remove_escape(MYBLOCKCHAIN *myblockchain,char *name);
unsigned int myblockchain_thread_safe(void);
my_bool myblockchain_embedded(void);
my_bool myblockchain_read_query_result(MYBLOCKCHAIN *myblockchain);
int myblockchain_reset_connection(MYBLOCKCHAIN *myblockchain);
enum enum_myblockchain_stmt_state
{
  MYBLOCKCHAIN_STMT_INIT_DONE= 1, MYBLOCKCHAIN_STMT_PREPARE_DONE, MYBLOCKCHAIN_STMT_EXECUTE_DONE,
  MYBLOCKCHAIN_STMT_FETCH_DONE
};
typedef struct st_myblockchain_bind
{
  unsigned long *length;
  my_bool *is_null;
  void *buffer;
  my_bool *error;
  unsigned char *row_ptr;
  void (*store_param_func)(NET *net, struct st_myblockchain_bind *param);
  void (*fetch_result)(struct st_myblockchain_bind *, MYBLOCKCHAIN_FIELD *,
                       unsigned char **row);
  void (*skip_result)(struct st_myblockchain_bind *, MYBLOCKCHAIN_FIELD *,
        unsigned char **row);
  unsigned long buffer_length;
  unsigned long offset;
  unsigned long length_value;
  unsigned int param_number;
  unsigned int pack_length;
  enum enum_field_types buffer_type;
  my_bool error_value;
  my_bool is_unsigned;
  my_bool long_data_used;
  my_bool is_null_value;
  void *extension;
} MYBLOCKCHAIN_BIND;
struct st_myblockchain_stmt_extension;
typedef struct st_myblockchain_stmt
{
  MEM_ROOT mem_root;
  LIST list;
  MYBLOCKCHAIN *myblockchain;
  MYBLOCKCHAIN_BIND *params;
  MYBLOCKCHAIN_BIND *bind;
  MYBLOCKCHAIN_FIELD *fields;
  MYBLOCKCHAIN_DATA result;
  MYBLOCKCHAIN_ROWS *data_cursor;
  int (*read_row_func)(struct st_myblockchain_stmt *stmt,
                                  unsigned char **row);
  my_ulonglong affected_rows;
  my_ulonglong insert_id;
  unsigned long stmt_id;
  unsigned long flags;
  unsigned long prefetch_rows;
  unsigned int server_status;
  unsigned int last_errno;
  unsigned int param_count;
  unsigned int field_count;
  enum enum_myblockchain_stmt_state state;
  char last_error[512];
  char sqlstate[5 +1];
  my_bool send_types_to_server;
  my_bool bind_param_done;
  unsigned char bind_result_done;
  my_bool unbuffered_fetch_cancelled;
  my_bool update_max_length;
  struct st_myblockchain_stmt_extension *extension;
} MYBLOCKCHAIN_STMT;
enum enum_stmt_attr_type
{
  STMT_ATTR_UPDATE_MAX_LENGTH,
  STMT_ATTR_CURSOR_TYPE,
  STMT_ATTR_PREFETCH_ROWS
};
MYBLOCKCHAIN_STMT * myblockchain_stmt_init(MYBLOCKCHAIN *myblockchain);
int myblockchain_stmt_prepare(MYBLOCKCHAIN_STMT *stmt, const char *query,
                               unsigned long length);
int myblockchain_stmt_execute(MYBLOCKCHAIN_STMT *stmt);
int myblockchain_stmt_fetch(MYBLOCKCHAIN_STMT *stmt);
int myblockchain_stmt_fetch_column(MYBLOCKCHAIN_STMT *stmt, MYBLOCKCHAIN_BIND *bind_arg,
                                    unsigned int column,
                                    unsigned long offset);
int myblockchain_stmt_store_result(MYBLOCKCHAIN_STMT *stmt);
unsigned long myblockchain_stmt_param_count(MYBLOCKCHAIN_STMT * stmt);
my_bool myblockchain_stmt_attr_set(MYBLOCKCHAIN_STMT *stmt,
                                    enum enum_stmt_attr_type attr_type,
                                    const void *attr);
my_bool myblockchain_stmt_attr_get(MYBLOCKCHAIN_STMT *stmt,
                                    enum enum_stmt_attr_type attr_type,
                                    void *attr);
my_bool myblockchain_stmt_bind_param(MYBLOCKCHAIN_STMT * stmt, MYBLOCKCHAIN_BIND * bnd);
my_bool myblockchain_stmt_bind_result(MYBLOCKCHAIN_STMT * stmt, MYBLOCKCHAIN_BIND * bnd);
my_bool myblockchain_stmt_close(MYBLOCKCHAIN_STMT * stmt);
my_bool myblockchain_stmt_reset(MYBLOCKCHAIN_STMT * stmt);
my_bool myblockchain_stmt_free_result(MYBLOCKCHAIN_STMT *stmt);
my_bool myblockchain_stmt_send_long_data(MYBLOCKCHAIN_STMT *stmt,
                                          unsigned int param_number,
                                          const char *data,
                                          unsigned long length);
MYBLOCKCHAIN_RES * myblockchain_stmt_result_metadata(MYBLOCKCHAIN_STMT *stmt);
MYBLOCKCHAIN_RES * myblockchain_stmt_param_metadata(MYBLOCKCHAIN_STMT *stmt);
unsigned int myblockchain_stmt_errno(MYBLOCKCHAIN_STMT * stmt);
const char * myblockchain_stmt_error(MYBLOCKCHAIN_STMT * stmt);
const char * myblockchain_stmt_sqlstate(MYBLOCKCHAIN_STMT * stmt);
MYBLOCKCHAIN_ROW_OFFSET myblockchain_stmt_row_seek(MYBLOCKCHAIN_STMT *stmt,
                                             MYBLOCKCHAIN_ROW_OFFSET offset);
MYBLOCKCHAIN_ROW_OFFSET myblockchain_stmt_row_tell(MYBLOCKCHAIN_STMT *stmt);
void myblockchain_stmt_data_seek(MYBLOCKCHAIN_STMT *stmt, my_ulonglong offset);
my_ulonglong myblockchain_stmt_num_rows(MYBLOCKCHAIN_STMT *stmt);
my_ulonglong myblockchain_stmt_affected_rows(MYBLOCKCHAIN_STMT *stmt);
my_ulonglong myblockchain_stmt_insert_id(MYBLOCKCHAIN_STMT *stmt);
unsigned int myblockchain_stmt_field_count(MYBLOCKCHAIN_STMT *stmt);
my_bool myblockchain_commit(MYBLOCKCHAIN * myblockchain);
my_bool myblockchain_rollback(MYBLOCKCHAIN * myblockchain);
my_bool myblockchain_autocommit(MYBLOCKCHAIN * myblockchain, my_bool auto_mode);
my_bool myblockchain_more_results(MYBLOCKCHAIN *myblockchain);
int myblockchain_next_result(MYBLOCKCHAIN *myblockchain);
int myblockchain_stmt_next_result(MYBLOCKCHAIN_STMT *stmt);
void myblockchain_close(MYBLOCKCHAIN *sock);
