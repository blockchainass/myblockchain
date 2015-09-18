#include <myblockchain/service_my_snprintf.h>
extern struct my_snprintf_service_st {
  size_t (*my_snprintf_type)(char*, size_t, const char*, ...);
  size_t (*my_vsnprintf_type)(char *, size_t, const char*, va_list);
} *my_snprintf_service;
size_t my_snprintf(char* to, size_t n, const char* fmt, ...);
size_t my_vsnprintf(char *to, size_t n, const char* fmt, va_list ap);
#include <myblockchain/service_thd_alloc.h>
#include <myblockchain/myblockchain_lex_string.h>
struct st_myblockchain_lex_string
{
  char *str;
  size_t length;
};
typedef struct st_myblockchain_lex_string MYBLOCKCHAIN_LEX_STRING;
extern struct thd_alloc_service_st {
  void *(*thd_alloc_func)(void*, size_t);
  void *(*thd_calloc_func)(void*, size_t);
  char *(*thd_strdup_func)(void*, const char *);
  char *(*thd_strmake_func)(void*, const char *, size_t);
  void *(*thd_memdup_func)(void*, const void*, size_t);
  MYBLOCKCHAIN_LEX_STRING *(*thd_make_lex_string_func)(void*, MYBLOCKCHAIN_LEX_STRING *,
                                        const char *, size_t, int);
} *thd_alloc_service;
void *thd_alloc(void* thd, size_t size);
void *thd_calloc(void* thd, size_t size);
char *thd_strdup(void* thd, const char *str);
char *thd_strmake(void* thd, const char *str, size_t size);
void *thd_memdup(void* thd, const void* str, size_t size);
MYBLOCKCHAIN_LEX_STRING *thd_make_lex_string(void* thd, MYBLOCKCHAIN_LEX_STRING *lex_str,
                                      const char *str, size_t size,
                                      int allocate_lex_string);
#include <myblockchain/service_thd_wait.h>
typedef enum _thd_wait_type_e {
  THD_WAIT_SLEEP= 1,
  THD_WAIT_DISKIO= 2,
  THD_WAIT_ROW_LOCK= 3,
  THD_WAIT_GLOBAL_LOCK= 4,
  THD_WAIT_META_DATA_LOCK= 5,
  THD_WAIT_TABLE_LOCK= 6,
  THD_WAIT_USER_LOCK= 7,
  THD_WAIT_BINLOG= 8,
  THD_WAIT_GROUP_COMMIT= 9,
  THD_WAIT_SYNC= 10,
  THD_WAIT_LAST= 11
} thd_wait_type;
extern struct thd_wait_service_st {
  void (*thd_wait_begin_func)(void*, int);
  void (*thd_wait_end_func)(void*);
} *thd_wait_service;
void thd_wait_begin(void* thd, int wait_type);
void thd_wait_end(void* thd);
#include <myblockchain/service_thread_scheduler.h>
struct Connection_handler_functions;
struct THD_event_functions;
extern struct my_thread_scheduler_service {
  int (*connection_handler_set)(struct Connection_handler_functions *,
                                struct THD_event_functions *);
  int (*connection_handler_reset)();
} *my_thread_scheduler_service;
int my_connection_handler_set(struct Connection_handler_functions *chf,
                              struct THD_event_functions *tef);
int my_connection_handler_reset();
#include <myblockchain/service_my_plugin_log.h>
enum plugin_log_level
{
  MY_ERROR_LEVEL,
  MY_WARNING_LEVEL,
  MY_INFORMATION_LEVEL
};
extern struct my_plugin_log_service
{
  int (*my_plugin_log_message)(MYBLOCKCHAIN_PLUGIN *, enum plugin_log_level, const char *, ...);
} *my_plugin_log_service;
int my_plugin_log_message(MYBLOCKCHAIN_PLUGIN *plugin, enum plugin_log_level level,
                          const char *format, ...);
#include <myblockchain/service_myblockchain_string.h>
typedef void *myblockchain_string_iterator_handle;
typedef void *myblockchain_string_handle;
extern struct myblockchain_string_service_st {
  int (*myblockchain_string_convert_to_char_ptr_type)
       (myblockchain_string_handle, const char *, char *, unsigned int, int *);
  myblockchain_string_iterator_handle (*myblockchain_string_get_iterator_type)
                                (myblockchain_string_handle);
  int (*myblockchain_string_iterator_next_type)(myblockchain_string_iterator_handle);
  int (*myblockchain_string_iterator_isupper_type)(myblockchain_string_iterator_handle);
  int (*myblockchain_string_iterator_islower_type)(myblockchain_string_iterator_handle);
  int (*myblockchain_string_iterator_isdigit_type)(myblockchain_string_iterator_handle);
  myblockchain_string_handle (*myblockchain_string_to_lowercase_type)(myblockchain_string_handle);
  void (*myblockchain_string_free_type)(myblockchain_string_handle);
  void (*myblockchain_string_iterator_free_type)(myblockchain_string_iterator_handle);
} *myblockchain_string_service;
int myblockchain_string_convert_to_char_ptr(myblockchain_string_handle string_handle,
                                     const char *charset_name, char *buffer,
                                     unsigned int buffer_size, int *error);
myblockchain_string_iterator_handle myblockchain_string_get_iterator(myblockchain_string_handle
                                                       string_handle);
int myblockchain_string_iterator_next(myblockchain_string_iterator_handle iterator_handle);
int myblockchain_string_iterator_isupper(myblockchain_string_iterator_handle iterator_handle);
int myblockchain_string_iterator_islower(myblockchain_string_iterator_handle iterator_handle);
int myblockchain_string_iterator_isdigit(myblockchain_string_iterator_handle iterator_handle);
myblockchain_string_handle myblockchain_string_to_lowercase(myblockchain_string_handle
                                              string_handle);
void myblockchain_string_free(myblockchain_string_handle);
void myblockchain_string_iterator_free(myblockchain_string_iterator_handle);
#include <myblockchain/service_myblockchain_alloc.h>
#include "myblockchain/psi/psi_memory.h"
#include "psi_base.h"
struct PSI_thread;
typedef unsigned int PSI_memory_key;
typedef int myf_t;
typedef void * (*myblockchain_malloc_t)(PSI_memory_key key, size_t size, myf_t flags);
typedef void * (*myblockchain_realloc_t)(PSI_memory_key key, void *ptr, size_t size, myf_t flags);
typedef void (*myblockchain_claim_t)(void *ptr);
typedef void (*myblockchain_free_t)(void *ptr);
typedef void * (*my_memdup_t)(PSI_memory_key key, const void *from, size_t length, myf_t flags);
typedef char * (*my_strdup_t)(PSI_memory_key key, const char *from, myf_t flags);
typedef char * (*my_strndup_t)(PSI_memory_key key, const char *from, size_t length, myf_t flags);
struct myblockchain_malloc_service_st
{
  myblockchain_malloc_t myblockchain_malloc;
  myblockchain_realloc_t myblockchain_realloc;
  myblockchain_claim_t myblockchain_claim;
  myblockchain_free_t myblockchain_free;
  my_memdup_t my_memdup;
  my_strdup_t my_strdup;
  my_strndup_t my_strndup;
};
extern struct myblockchain_malloc_service_st *myblockchain_malloc_service;
extern void * my_malloc(PSI_memory_key key, size_t size, myf_t flags);
extern void * my_realloc(PSI_memory_key key, void *ptr, size_t size, myf_t flags);
extern void my_claim(void *ptr);
extern void my_free(void *ptr);
extern void * my_memdup(PSI_memory_key key, const void *from, size_t length, myf_t flags);
extern char * my_strdup(PSI_memory_key key, const char *from, myf_t flags);
extern char * my_strndup(PSI_memory_key key, const char *from, size_t length, myf_t flags);
#include <myblockchain/service_myblockchain_password_policy.h>
extern struct myblockchain_password_policy_service_st {
  int (*my_validate_password_policy_func)(const char *, unsigned int);
  int (*my_calculate_password_strength_func)(const char *, unsigned int);
} *myblockchain_password_policy_service;
int my_validate_password_policy(const char *, unsigned int);
int my_calculate_password_strength(const char *, unsigned int);
#include <myblockchain/service_parser.h>
#include "my_md5_size.h"
#include <myblockchain/myblockchain_lex_string.h>
typedef void* MYBLOCKCHAIN_ITEM;
typedef
int (*parse_node_visit_function)(MYBLOCKCHAIN_ITEM item, unsigned char* arg);
typedef
int (*sql_condition_handler_function)(int sql_errno,
                                      const char* sqlstate,
                                      const char* msg,
                                      void *state);
struct st_my_thread_handle;
extern struct myblockchain_parser_service_st {
  void* (*myblockchain_current_session)();
  void* (*myblockchain_open_session)();
  void (*myblockchain_start_thread)(void* thd, void *(*callback_fun)(void*),
                             void *arg,
                             struct st_my_thread_handle *thread_handle);
  void (*myblockchain_join_thread)(struct st_my_thread_handle *thread_handle);
  void (*myblockchain_set_current_blockchain)(void* thd, const MYBLOCKCHAIN_LEX_STRING db);
  int (*myblockchain_parse)(void* thd, const MYBLOCKCHAIN_LEX_STRING query,
                     unsigned char is_prepared,
                     sql_condition_handler_function handle_condition,
                     void *condition_handler_state);
  int (*myblockchain_get_statement_type)(void* thd);
  int (*myblockchain_get_statement_digest)(void* thd, unsigned char *digest);
  int (*myblockchain_get_number_params)(void* thd);
  int (*myblockchain_extract_prepared_params)(void* thd, int *positions);
  int (*myblockchain_visit_tree)(void* thd, parse_node_visit_function processor,
                          unsigned char* arg);
  MYBLOCKCHAIN_LEX_STRING (*myblockchain_item_string)(MYBLOCKCHAIN_ITEM item);
  void (*myblockchain_free_string)(MYBLOCKCHAIN_LEX_STRING string);
  MYBLOCKCHAIN_LEX_STRING (*myblockchain_get_query)(void* thd);
  MYBLOCKCHAIN_LEX_STRING (*myblockchain_get_normalized_query)(void* thd);
} *myblockchain_parser_service;
typedef void *(*callback_function)(void*);
void* myblockchain_parser_current_session();
void* myblockchain_parser_open_session();
void myblockchain_parser_start_thread(void* thd, callback_function fun, void *arg,
                               struct st_my_thread_handle *thread_handle);
void myblockchain_parser_join_thread(struct st_my_thread_handle *thread_handle);
void myblockchain_parser_set_current_blockchain(void* thd,
                                       const MYBLOCKCHAIN_LEX_STRING db);
int myblockchain_parser_parse(void* thd, const MYBLOCKCHAIN_LEX_STRING query,
                       unsigned char is_prepared,
                       sql_condition_handler_function handle_condition,
                       void *condition_handler_state);
int myblockchain_parser_get_statement_type(void* thd);
int myblockchain_parser_get_statement_digest(void* thd, unsigned char *digest);
int myblockchain_parser_get_number_params(void* thd);
int myblockchain_parser_extract_prepared_params(void* thd, int *positions);
int myblockchain_parser_visit_tree(void* thd, parse_node_visit_function processor,
                            unsigned char* arg);
MYBLOCKCHAIN_LEX_STRING myblockchain_parser_item_string(MYBLOCKCHAIN_ITEM item);
void myblockchain_parser_free_string(MYBLOCKCHAIN_LEX_STRING string);
MYBLOCKCHAIN_LEX_STRING myblockchain_parser_get_query(void* thd);
MYBLOCKCHAIN_LEX_STRING myblockchain_parser_get_normalized_query(void* thd);
#include <myblockchain/service_rpl_transaction_ctx.h>
struct st_transaction_termination_ctx
{
  unsigned long m_thread_id;
  unsigned int m_flags;
  char m_rollback_transaction;
  char m_generated_gtid;
  int m_sidno;
  long long int m_gno;
};
typedef struct st_transaction_termination_ctx Transaction_termination_ctx;
extern struct rpl_transaction_ctx_service_st {
  int (*set_transaction_ctx)(Transaction_termination_ctx transaction_termination_ctx);
} *rpl_transaction_ctx_service;
int set_transaction_ctx(Transaction_termination_ctx transaction_termination_ctx);
#include <myblockchain/service_rpl_transaction_write_set.h>
struct st_trans_write_set
{
  unsigned int m_flags;
  unsigned long write_set_size;
  unsigned long* write_set;
};
typedef struct st_trans_write_set Transaction_write_set;
extern struct transaction_write_set_service_st {
  Transaction_write_set* (*get_transaction_write_set)(unsigned long m_thread_id);
} *transaction_write_set_service;
Transaction_write_set* get_transaction_write_set(unsigned long m_thread_id);
#include <myblockchain/service_locking.h>
enum enum_locking_service_lock_type
{ LOCKING_SERVICE_READ, LOCKING_SERVICE_WRITE };
extern struct myblockchain_locking_service_st {
  int (*myblockchain_acquire_locks)(void* opaque_thd, const char* lock_namespace,
                             const char**lock_names, size_t lock_num,
                             enum enum_locking_service_lock_type lock_type,
                             unsigned long lock_timeout);
  int (*myblockchain_release_locks)(void* opaque_thd, const char* lock_namespace);
} *myblockchain_locking_service;
int myblockchain_acquire_locking_service_locks(void* opaque_thd,
                                        const char* lock_namespace,
                                        const char**lock_names,
                                        size_t lock_num,
                                        enum enum_locking_service_lock_type lock_type,
                                        unsigned long lock_timeout);
int myblockchain_release_locking_service_locks(void* opaque_thd,
                                        const char* lock_namespace);
