#include "plugin.h"
typedef void * MYBLOCKCHAIN_PLUGIN;
struct st_myblockchain_xid {
  long formatID;
  long gtrid_length;
  long bqual_length;
  char data[128];
};
typedef struct st_myblockchain_xid MYBLOCKCHAIN_XID;
enum enum_myblockchain_show_type
{
  SHOW_UNDEF, SHOW_BOOL,
  SHOW_INT,
  SHOW_LONG,
  SHOW_LONGLONG,
  SHOW_CHAR, SHOW_CHAR_PTR,
  SHOW_ARRAY, SHOW_FUNC, SHOW_DOUBLE
};
enum enum_myblockchain_show_scope
{
  SHOW_SCOPE_UNDEF,
  SHOW_SCOPE_GLOBAL
};
struct st_myblockchain_show_var
{
  const char *name;
  char *value;
  enum enum_myblockchain_show_type type;
  enum enum_myblockchain_show_scope scope;
};
typedef int (*myblockchain_show_var_func)(void*, struct st_myblockchain_show_var*, char *);
struct st_myblockchain_sys_var;
struct st_myblockchain_value;
typedef int (*myblockchain_var_check_func)(void* thd,
                                    struct st_myblockchain_sys_var *var,
                                    void *save, struct st_myblockchain_value *value);
typedef void (*myblockchain_var_update_func)(void* thd,
                                      struct st_myblockchain_sys_var *var,
                                      void *var_ptr, const void *save);
struct st_myblockchain_plugin
{
  int type;
  void *info;
  const char *name;
  const char *author;
  const char *descr;
  int license;
  int (*init)(MYBLOCKCHAIN_PLUGIN);
  int (*deinit)(MYBLOCKCHAIN_PLUGIN);
  unsigned int version;
  struct st_myblockchain_show_var *status_vars;
  struct st_myblockchain_sys_var **system_vars;
  void * __reserved1;
  unsigned long flags;
};
struct st_myblockchain_daemon
{
  int interface_version;
};
struct st_myblockchain_information_schema
{
  int interface_version;
};
struct st_myblockchain_storage_engine
{
  int interface_version;
};
struct handlerton;
 struct Mysql_replication {
   int interface_version;
 };
struct st_myblockchain_value
{
  int (*value_type)(struct st_myblockchain_value *);
  const char *(*val_str)(struct st_myblockchain_value *, char *buffer, int *length);
  int (*val_real)(struct st_myblockchain_value *, double *realbuf);
  int (*val_int)(struct st_myblockchain_value *, long long *intbuf);
  int (*is_unsigned)(struct st_myblockchain_value *);
};
int thd_in_lock_tables(const void* thd);
int thd_tablespace_op(const void* thd);
long long thd_test_options(const void* thd, long long test_options);
int thd_sql_command(const void* thd);
const char *set_thd_proc_info(void* thd, const char *info,
                              const char *calling_func,
                              const char *calling_file,
                              const unsigned int calling_line);
void **thd_ha_data(const void* thd, const struct handlerton *hton);
void thd_storage_lock_wait(void* thd, long long value);
int thd_tx_isolation(const void* thd);
int thd_tx_is_read_only(const void* thd);
void* thd_tx_arbitrate(void* requestor, void* holder);
int thd_tx_priority(const void* thd);
int thd_tx_is_dd_trx(const void* thd);
char *thd_security_context(void* thd, char *buffer, size_t length,
                           size_t max_query_len);
void thd_inc_row_count(void* thd);
int thd_allow_batch(void* thd);
void thd_mark_transaction_to_rollback(void* thd, int all);
int myblockchain_tmpfile(const char *prefix);
int thd_killed(const void* thd);
void thd_set_kill_status(const void* thd);
void thd_binlog_pos(const void* thd,
                    const char **file_var,
                    unsigned long long *pos_var);
unsigned long thd_get_thread_id(const void* thd);
void thd_get_xid(const void* thd, MYBLOCKCHAIN_XID *xid);
void myblockchain_query_cache_invalidate4(void* thd,
                                   const char *key, unsigned int key_length,
                                   int using_trx);
void *thd_get_ha_data(const void* thd, const struct handlerton *hton);
void thd_set_ha_data(void* thd, const struct handlerton *hton,
                     const void *ha_data);
enum enum_ftparser_mode
{
  MYBLOCKCHAIN_FTPARSER_SIMPLE_MODE= 0,
  MYBLOCKCHAIN_FTPARSER_WITH_STOPWORDS= 1,
  MYBLOCKCHAIN_FTPARSER_FULL_BOOLEAN_INFO= 2
};
enum enum_ft_token_type
{
  FT_TOKEN_EOF= 0,
  FT_TOKEN_WORD= 1,
  FT_TOKEN_LEFT_PAREN= 2,
  FT_TOKEN_RIGHT_PAREN= 3,
  FT_TOKEN_STOPWORD= 4
};
typedef struct st_myblockchain_ftparser_boolean_info
{
  enum enum_ft_token_type type;
  int yesno;
  int weight_adjust;
  char wasign;
  char trunc;
  int position;
  char prev;
  char *quot;
} MYBLOCKCHAIN_FTPARSER_BOOLEAN_INFO;
typedef struct st_myblockchain_ftparser_param
{
  int (*myblockchain_parse)(struct st_myblockchain_ftparser_param *,
                     char *doc, int doc_len);
  int (*myblockchain_add_word)(struct st_myblockchain_ftparser_param *,
                        char *word, int word_len,
                        MYBLOCKCHAIN_FTPARSER_BOOLEAN_INFO *boolean_info);
  void *ftparser_state;
  void *myblockchain_ftparam;
  const struct charset_info_st *cs;
  char *doc;
  int length;
  int flags;
  enum enum_ftparser_mode mode;
} MYBLOCKCHAIN_FTPARSER_PARAM;
struct st_myblockchain_ftparser
{
  int interface_version;
  int (*parse)(MYBLOCKCHAIN_FTPARSER_PARAM *param);
  int (*init)(MYBLOCKCHAIN_FTPARSER_PARAM *param);
  int (*deinit)(MYBLOCKCHAIN_FTPARSER_PARAM *param);
};
