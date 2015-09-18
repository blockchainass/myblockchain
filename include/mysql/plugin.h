/* Copyright (c) 2005, 2015, Oracle and/or its affiliates. All rights reserved.

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

#ifndef _my_plugin_h
#define _my_plugin_h

#ifndef MYBLOCKCHAIN_ABI_CHECK
#include <stddef.h>
#include "myblockchain_version.h" /* MYBLOCKCHAIN_VERSION_ID */
#endif

/*
  On Windows, exports from DLL need to be declared.
  Also, plugin needs to be declared as extern "C" because MSVC 
  unlike other compilers, uses C++ mangling for variables not only
  for functions.
*/
#if defined(_MSC_VER)
#if defined(MYBLOCKCHAIN_DYNAMIC_PLUGIN)
  #ifdef __cplusplus
    #define MYBLOCKCHAIN_PLUGIN_EXPORT extern "C" __declspec(dllexport)
  #else
    #define MYBLOCKCHAIN_PLUGIN_EXPORT __declspec(dllexport)
  #endif
#else /* MYBLOCKCHAIN_DYNAMIC_PLUGIN */
  #ifdef __cplusplus
    #define  MYBLOCKCHAIN_PLUGIN_EXPORT extern "C"
  #else
    #define MYBLOCKCHAIN_PLUGIN_EXPORT 
  #endif
#endif /*MYBLOCKCHAIN_DYNAMIC_PLUGIN */
#else /*_MSC_VER */
#define MYBLOCKCHAIN_PLUGIN_EXPORT
#endif

#ifdef __cplusplus
class THD;
class Item;
#define MYBLOCKCHAIN_THD THD*
#else
#define MYBLOCKCHAIN_THD void*
#endif

typedef void * MYBLOCKCHAIN_PLUGIN;

#ifndef MYBLOCKCHAIN_ABI_CHECK
#include <myblockchain/services.h>
#endif

#define MYBLOCKCHAIN_XIDDATASIZE 128
/**
  struct st_myblockchain_xid is binary compatible with the XID structure as
  in the X/Open CAE Specification, Distributed Transaction Processing:
  The XA Specification, X/Open Company Ltd., 1991.
  http://www.opengroup.org/bookstore/catalog/c193.htm

  @see XID in sql/handler.h
*/
struct st_myblockchain_xid {
  long formatID;
  long gtrid_length;
  long bqual_length;
  char data[MYBLOCKCHAIN_XIDDATASIZE];  /* Not \0-terminated */
};
typedef struct st_myblockchain_xid MYBLOCKCHAIN_XID;

/*************************************************************************
  Plugin API. Common for all plugin types.
*/

#define MYBLOCKCHAIN_PLUGIN_INTERFACE_VERSION 0x0106

/*
  The allowable types of plugins
*/
#define MYBLOCKCHAIN_UDF_PLUGIN             0  /* User-defined function        */
#define MYBLOCKCHAIN_STORAGE_ENGINE_PLUGIN  1  /* Storage Engine               */
#define MYBLOCKCHAIN_FTPARSER_PLUGIN        2  /* Full-text parser plugin      */
#define MYBLOCKCHAIN_DAEMON_PLUGIN          3  /* The daemon/raw plugin type */
#define MYBLOCKCHAIN_INFORMATION_SCHEMA_PLUGIN  4  /* The I_S plugin type */
#define MYBLOCKCHAIN_AUDIT_PLUGIN           5  /* The Audit plugin type        */
#define MYBLOCKCHAIN_REPLICATION_PLUGIN     6	/* The replication plugin type */
#define MYBLOCKCHAIN_AUTHENTICATION_PLUGIN  7  /* The authentication plugin type */
#define MYBLOCKCHAIN_VALIDATE_PASSWORD_PLUGIN  8   /* validate password plugin type */
#define MYBLOCKCHAIN_GROUP_REPLICATION_PLUGIN  9  /* The Group Replication plugin */
#define MYBLOCKCHAIN_MAX_PLUGIN_TYPE_NUM    10  /* The number of plugin types   */

/* We use the following strings to define licenses for plugins */
#define PLUGIN_LICENSE_PROPRIETARY 0
#define PLUGIN_LICENSE_GPL 1
#define PLUGIN_LICENSE_BSD 2

#define PLUGIN_LICENSE_PROPRIETARY_STRING "PROPRIETARY"
#define PLUGIN_LICENSE_GPL_STRING "GPL"
#define PLUGIN_LICENSE_BSD_STRING "BSD"

/*
  Macros for beginning and ending plugin declarations.  Between
  myblockchain_declare_plugin and myblockchain_declare_plugin_end there should
  be a st_myblockchain_plugin struct for each plugin to be declared.
*/


#ifndef MYBLOCKCHAIN_DYNAMIC_PLUGIN
#define __MYBLOCKCHAIN_DECLARE_PLUGIN(NAME, VERSION, PSIZE, DECLS)                   \
MYBLOCKCHAIN_PLUGIN_EXPORT int VERSION= MYBLOCKCHAIN_PLUGIN_INTERFACE_VERSION;                                  \
MYBLOCKCHAIN_PLUGIN_EXPORT int PSIZE= sizeof(struct st_myblockchain_plugin);                                    \
MYBLOCKCHAIN_PLUGIN_EXPORT struct st_myblockchain_plugin DECLS[]= {
#else
#define __MYBLOCKCHAIN_DECLARE_PLUGIN(NAME, VERSION, PSIZE, DECLS)                   \
MYBLOCKCHAIN_PLUGIN_EXPORT int _myblockchain_plugin_interface_version_= MYBLOCKCHAIN_PLUGIN_INTERFACE_VERSION;         \
MYBLOCKCHAIN_PLUGIN_EXPORT int _myblockchain_sizeof_struct_st_plugin_= sizeof(struct st_myblockchain_plugin);          \
MYBLOCKCHAIN_PLUGIN_EXPORT struct st_myblockchain_plugin _myblockchain_plugin_declarations_[]= {
#endif

#define myblockchain_declare_plugin(NAME) \
__MYBLOCKCHAIN_DECLARE_PLUGIN(NAME, \
                 builtin_ ## NAME ## _plugin_interface_version, \
                 builtin_ ## NAME ## _sizeof_struct_st_plugin, \
                 builtin_ ## NAME ## _plugin)

#define myblockchain_declare_plugin_end ,{0,0,0,0,0,0,0,0,0,0,0,0,0}}

/**
  Declarations for SHOW STATUS support in plugins
*/
enum enum_myblockchain_show_type
{
  SHOW_UNDEF, SHOW_BOOL,
  SHOW_INT,        ///< shown as _unsigned_ int
  SHOW_LONG,       ///< shown as _unsigned_ long
  SHOW_LONGLONG,   ///< shown as _unsigned_ longlong
  SHOW_CHAR, SHOW_CHAR_PTR,
  SHOW_ARRAY, SHOW_FUNC, SHOW_DOUBLE
#ifdef MYBLOCKCHAIN_SERVER
  /*
    This include defines server-only values of the enum.
    Using them in plugins is not supported.
  */
  #include "sql_plugin_enum.h"
#endif
};

/**
  Status variable scope.
  Only GLOBAL status variable scope is available in plugins.
*/
enum enum_myblockchain_show_scope
{
  SHOW_SCOPE_UNDEF,
  SHOW_SCOPE_GLOBAL
#ifdef MYBLOCKCHAIN_SERVER
  /* Server-only values. Not supported in plugins. */
  ,
  SHOW_SCOPE_SESSION,
  SHOW_SCOPE_ALL
#endif
};

/**
  SHOW STATUS Server status variable
*/
struct st_myblockchain_show_var
{
  const char *name;
  char *value;
  enum enum_myblockchain_show_type type;
  enum enum_myblockchain_show_scope scope;
};

#define SHOW_VAR_MAX_NAME_LEN 64
#define SHOW_VAR_FUNC_BUFF_SIZE 1024
typedef int (*myblockchain_show_var_func)(MYBLOCKCHAIN_THD, struct st_myblockchain_show_var*, char *);


/*
  Constants for plugin flags.
 */

#define PLUGIN_OPT_NO_INSTALL   1UL   /* Not dynamically loadable */
#define PLUGIN_OPT_NO_UNINSTALL 2UL   /* Not dynamically unloadable */


/*
  declarations for server variables and command line options
*/


#define PLUGIN_VAR_BOOL         0x0001
#define PLUGIN_VAR_INT          0x0002
#define PLUGIN_VAR_LONG         0x0003
#define PLUGIN_VAR_LONGLONG     0x0004
#define PLUGIN_VAR_STR          0x0005
#define PLUGIN_VAR_ENUM         0x0006
#define PLUGIN_VAR_SET          0x0007
#define PLUGIN_VAR_DOUBLE       0x0008
#define PLUGIN_VAR_UNSIGNED     0x0080
#define PLUGIN_VAR_THDLOCAL     0x0100 /* Variable is per-connection */
#define PLUGIN_VAR_READONLY     0x0200 /* Server variable is read only */
#define PLUGIN_VAR_NOSYSVAR     0x0400 /* Not a server variable */
#define PLUGIN_VAR_NOCMDOPT     0x0800 /* Not a command line option */
#define PLUGIN_VAR_NOCMDARG     0x1000 /* No argument for cmd line */
#define PLUGIN_VAR_RQCMDARG     0x0000 /* Argument required for cmd line */
#define PLUGIN_VAR_OPCMDARG     0x2000 /* Argument optional for cmd line */
#define PLUGIN_VAR_MEMALLOC     0x8000 /* String needs memory allocated */

struct st_myblockchain_sys_var;
struct st_myblockchain_value;

/*
  SYNOPSIS
    (*myblockchain_var_check_func)()
      thd               thread handle
      var               dynamic variable being altered
      save              pointer to temporary storage
      value             user provided value
  RETURN
    0   user provided value is OK and the update func may be called.
    any other value indicates error.
  
  This function should parse the user provided value and store in the
  provided temporary storage any data as required by the update func.
  There is sufficient space in the temporary storage to store a double.
  Note that the update func may not be called if any other error occurs
  so any memory allocated should be thread-local so that it may be freed
  automatically at the end of the statement.
*/

typedef int (*myblockchain_var_check_func)(MYBLOCKCHAIN_THD thd,
                                    struct st_myblockchain_sys_var *var,
                                    void *save, struct st_myblockchain_value *value);

/*
  SYNOPSIS
    (*myblockchain_var_update_func)()
      thd               thread handle
      var               dynamic variable being altered
      var_ptr           pointer to dynamic variable
      save              pointer to temporary storage
   RETURN
     NONE
   
   This function should use the validated value stored in the temporary store
   and persist it in the provided pointer to the dynamic variable.
   For example, strings may require memory to be allocated.
*/
typedef void (*myblockchain_var_update_func)(MYBLOCKCHAIN_THD thd,
                                      struct st_myblockchain_sys_var *var,
                                      void *var_ptr, const void *save);


/* the following declarations are for internal use only */


#define PLUGIN_VAR_MASK \
        (PLUGIN_VAR_READONLY | PLUGIN_VAR_NOSYSVAR | \
         PLUGIN_VAR_NOCMDOPT | PLUGIN_VAR_NOCMDARG | \
         PLUGIN_VAR_OPCMDARG | PLUGIN_VAR_RQCMDARG | PLUGIN_VAR_MEMALLOC)

#define MYBLOCKCHAIN_PLUGIN_VAR_HEADER \
  int flags;                    \
  const char *name;             \
  const char *comment;          \
  myblockchain_var_check_func check;   \
  myblockchain_var_update_func update

#define MYBLOCKCHAIN_SYSVAR_NAME(name) myblockchain_sysvar_ ## name
#define MYBLOCKCHAIN_SYSVAR(name) \
  ((struct st_myblockchain_sys_var *)&(MYBLOCKCHAIN_SYSVAR_NAME(name)))

/*
  for global variables, the value pointer is the first
  element after the header, the default value is the second.
  for thread variables, the value offset is the first
  element after the header, the default value is the second.
*/
   

#define DECLARE_MYBLOCKCHAIN_SYSVAR_BASIC(name, type) struct { \
  MYBLOCKCHAIN_PLUGIN_VAR_HEADER;      \
  type *value;                  \
  const type def_val;           \
} MYBLOCKCHAIN_SYSVAR_NAME(name)

#define DECLARE_MYBLOCKCHAIN_SYSVAR_SIMPLE(name, type) struct { \
  MYBLOCKCHAIN_PLUGIN_VAR_HEADER;      \
  type *value; type def_val;    \
  type min_val; type max_val;   \
  type blk_sz;                  \
} MYBLOCKCHAIN_SYSVAR_NAME(name)

#define DECLARE_MYBLOCKCHAIN_SYSVAR_TYPELIB(name, type) struct { \
  MYBLOCKCHAIN_PLUGIN_VAR_HEADER;      \
  type *value; type def_val;    \
  TYPELIB *typelib;             \
} MYBLOCKCHAIN_SYSVAR_NAME(name)

#define DECLARE_THDVAR_FUNC(type) \
  type *(*resolve)(MYBLOCKCHAIN_THD thd, int offset)

#define DECLARE_MYBLOCKCHAIN_THDVAR_BASIC(name, type) struct { \
  MYBLOCKCHAIN_PLUGIN_VAR_HEADER;      \
  int offset;                   \
  const type def_val;           \
  DECLARE_THDVAR_FUNC(type);    \
} MYBLOCKCHAIN_SYSVAR_NAME(name)

#define DECLARE_MYBLOCKCHAIN_THDVAR_SIMPLE(name, type) struct { \
  MYBLOCKCHAIN_PLUGIN_VAR_HEADER;      \
  int offset;                   \
  type def_val; type min_val;   \
  type max_val; type blk_sz;    \
  DECLARE_THDVAR_FUNC(type);    \
} MYBLOCKCHAIN_SYSVAR_NAME(name)

#define DECLARE_MYBLOCKCHAIN_THDVAR_TYPELIB(name, type) struct { \
  MYBLOCKCHAIN_PLUGIN_VAR_HEADER;      \
  int offset;                   \
  type def_val;                 \
  DECLARE_THDVAR_FUNC(type);    \
  TYPELIB *typelib;             \
} MYBLOCKCHAIN_SYSVAR_NAME(name)


/*
  the following declarations are for use by plugin implementors
*/

#define MYBLOCKCHAIN_SYSVAR_BOOL(name, varname, opt, comment, check, update, def) \
DECLARE_MYBLOCKCHAIN_SYSVAR_BASIC(name, char) = { \
  PLUGIN_VAR_BOOL | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def}

#define MYBLOCKCHAIN_SYSVAR_STR(name, varname, opt, comment, check, update, def) \
DECLARE_MYBLOCKCHAIN_SYSVAR_BASIC(name, char *) = { \
  PLUGIN_VAR_STR | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def}

#define MYBLOCKCHAIN_SYSVAR_INT(name, varname, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYBLOCKCHAIN_SYSVAR_SIMPLE(name, int) = { \
  PLUGIN_VAR_INT | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def, min, max, blk }

#define MYBLOCKCHAIN_SYSVAR_UINT(name, varname, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYBLOCKCHAIN_SYSVAR_SIMPLE(name, unsigned int) = { \
  PLUGIN_VAR_INT | PLUGIN_VAR_UNSIGNED | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def, min, max, blk }

#define MYBLOCKCHAIN_SYSVAR_LONG(name, varname, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYBLOCKCHAIN_SYSVAR_SIMPLE(name, long) = { \
  PLUGIN_VAR_LONG | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def, min, max, blk }

#define MYBLOCKCHAIN_SYSVAR_ULONG(name, varname, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYBLOCKCHAIN_SYSVAR_SIMPLE(name, unsigned long) = { \
  PLUGIN_VAR_LONG | PLUGIN_VAR_UNSIGNED | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def, min, max, blk }

#define MYBLOCKCHAIN_SYSVAR_LONGLONG(name, varname, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYBLOCKCHAIN_SYSVAR_SIMPLE(name, long long) = { \
  PLUGIN_VAR_LONGLONG | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def, min, max, blk }

#define MYBLOCKCHAIN_SYSVAR_ULONGLONG(name, varname, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYBLOCKCHAIN_SYSVAR_SIMPLE(name, unsigned long long) = { \
  PLUGIN_VAR_LONGLONG | PLUGIN_VAR_UNSIGNED | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def, min, max, blk }

#define MYBLOCKCHAIN_SYSVAR_ENUM(name, varname, opt, comment, check, update, def, typelib) \
DECLARE_MYBLOCKCHAIN_SYSVAR_TYPELIB(name, unsigned long) = { \
  PLUGIN_VAR_ENUM | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def, typelib }

#define MYBLOCKCHAIN_SYSVAR_SET(name, varname, opt, comment, check, update, def, typelib) \
DECLARE_MYBLOCKCHAIN_SYSVAR_TYPELIB(name, unsigned long long) = { \
  PLUGIN_VAR_SET | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def, typelib }

#define MYBLOCKCHAIN_SYSVAR_DOUBLE(name, varname, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYBLOCKCHAIN_SYSVAR_SIMPLE(name, double) = { \
  PLUGIN_VAR_DOUBLE | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, &varname, def, min, max, blk }

#define MYBLOCKCHAIN_THDVAR_BOOL(name, opt, comment, check, update, def) \
DECLARE_MYBLOCKCHAIN_THDVAR_BASIC(name, char) = { \
  PLUGIN_VAR_BOOL | PLUGIN_VAR_THDLOCAL | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, -1, def, NULL}

#define MYBLOCKCHAIN_THDVAR_STR(name, opt, comment, check, update, def) \
DECLARE_MYBLOCKCHAIN_THDVAR_BASIC(name, char *) = { \
  PLUGIN_VAR_STR | PLUGIN_VAR_THDLOCAL | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, -1, def, NULL}

#define MYBLOCKCHAIN_THDVAR_INT(name, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYBLOCKCHAIN_THDVAR_SIMPLE(name, int) = { \
  PLUGIN_VAR_INT | PLUGIN_VAR_THDLOCAL | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, -1, def, min, max, blk, NULL }

#define MYBLOCKCHAIN_THDVAR_UINT(name, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYBLOCKCHAIN_THDVAR_SIMPLE(name, unsigned int) = { \
  PLUGIN_VAR_INT | PLUGIN_VAR_THDLOCAL | PLUGIN_VAR_UNSIGNED | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, -1, def, min, max, blk, NULL }

#define MYBLOCKCHAIN_THDVAR_LONG(name, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYBLOCKCHAIN_THDVAR_SIMPLE(name, long) = { \
  PLUGIN_VAR_LONG | PLUGIN_VAR_THDLOCAL | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, -1, def, min, max, blk, NULL }

#define MYBLOCKCHAIN_THDVAR_ULONG(name, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYBLOCKCHAIN_THDVAR_SIMPLE(name, unsigned long) = { \
  PLUGIN_VAR_LONG | PLUGIN_VAR_THDLOCAL | PLUGIN_VAR_UNSIGNED | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, -1, def, min, max, blk, NULL }

#define MYBLOCKCHAIN_THDVAR_LONGLONG(name, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYBLOCKCHAIN_THDVAR_SIMPLE(name, long long) = { \
  PLUGIN_VAR_LONGLONG | PLUGIN_VAR_THDLOCAL | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, -1, def, min, max, blk, NULL }

#define MYBLOCKCHAIN_THDVAR_ULONGLONG(name, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYBLOCKCHAIN_THDVAR_SIMPLE(name, unsigned long long) = { \
  PLUGIN_VAR_LONGLONG | PLUGIN_VAR_THDLOCAL | PLUGIN_VAR_UNSIGNED | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, -1, def, min, max, blk, NULL }

#define MYBLOCKCHAIN_THDVAR_ENUM(name, opt, comment, check, update, def, typelib) \
DECLARE_MYBLOCKCHAIN_THDVAR_TYPELIB(name, unsigned long) = { \
  PLUGIN_VAR_ENUM | PLUGIN_VAR_THDLOCAL | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, -1, def, NULL, typelib }

#define MYBLOCKCHAIN_THDVAR_SET(name, opt, comment, check, update, def, typelib) \
DECLARE_MYBLOCKCHAIN_THDVAR_TYPELIB(name, unsigned long long) = { \
  PLUGIN_VAR_SET | PLUGIN_VAR_THDLOCAL | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, -1, def, NULL, typelib }

#define MYBLOCKCHAIN_THDVAR_DOUBLE(name, opt, comment, check, update, def, min, max, blk) \
DECLARE_MYBLOCKCHAIN_THDVAR_SIMPLE(name, double) = { \
  PLUGIN_VAR_DOUBLE | PLUGIN_VAR_THDLOCAL | ((opt) & PLUGIN_VAR_MASK), \
  #name, comment, check, update, -1, def, min, max, blk, NULL }

/* accessor macros */

#define SYSVAR(name) \
  (*(MYBLOCKCHAIN_SYSVAR_NAME(name).value))

/* when thd == null, result points to global value */
#define THDVAR(thd, name) \
  (*(MYBLOCKCHAIN_SYSVAR_NAME(name).resolve(thd, MYBLOCKCHAIN_SYSVAR_NAME(name).offset)))


/*
  Plugin description structure.
*/

struct st_myblockchain_plugin
{
  int type;             /* the plugin type (a MYBLOCKCHAIN_XXX_PLUGIN value)   */
  void *info;           /* pointer to type-specific plugin descriptor   */
  const char *name;     /* plugin name                                  */
  const char *author;   /* plugin author (for I_S.PLUGINS)              */
  const char *descr;    /* general descriptive text (for I_S.PLUGINS)   */
  int license;          /* the plugin license (PLUGIN_LICENSE_XXX)      */
  int (*init)(MYBLOCKCHAIN_PLUGIN);  /* the function to invoke when plugin is loaded */
  int (*deinit)(MYBLOCKCHAIN_PLUGIN);/* the function to invoke when plugin is unloaded */
  unsigned int version; /* plugin version (for I_S.PLUGINS)             */
  struct st_myblockchain_show_var *status_vars;
  struct st_myblockchain_sys_var **system_vars;
  void * __reserved1;   /* reserved for dependency checking             */
  unsigned long flags;  /* flags for plugin */
};

/*************************************************************************
  API for Full-text parser plugin. (MYBLOCKCHAIN_FTPARSER_PLUGIN)
*/
#define MYBLOCKCHAIN_FTPARSER_INTERFACE_VERSION 0x0101

/*************************************************************************
  API for Query Rewrite plugin. (MYBLOCKCHAIN_QUERY_REWRITE_PLUGIN)
*/

#define MYBLOCKCHAIN_REWRITE_PRE_PARSE_INTERFACE_VERSION 0x0010
#define MYBLOCKCHAIN_REWRITE_POST_PARSE_INTERFACE_VERSION 0x0010

/*************************************************************************
  API for Storage Engine plugin. (MYBLOCKCHAIN_DAEMON_PLUGIN)
*/

/* handlertons of different MyBlockchain releases are incompatible */
#define MYBLOCKCHAIN_DAEMON_INTERFACE_VERSION (MYBLOCKCHAIN_VERSION_ID << 8)

/*
  Here we define only the descriptor structure, that is referred from
  st_myblockchain_plugin.
*/

struct st_myblockchain_daemon
{
  int interface_version;
};


/*************************************************************************
  API for I_S plugin. (MYBLOCKCHAIN_INFORMATION_SCHEMA_PLUGIN)
*/

/* handlertons of different MyBlockchain releases are incompatible */
#define MYBLOCKCHAIN_INFORMATION_SCHEMA_INTERFACE_VERSION (MYBLOCKCHAIN_VERSION_ID << 8)

/*
  Here we define only the descriptor structure, that is referred from
  st_myblockchain_plugin.
*/

struct st_myblockchain_information_schema
{
  int interface_version;
};


/*************************************************************************
  API for Storage Engine plugin. (MYBLOCKCHAIN_STORAGE_ENGINE_PLUGIN)
*/

/* handlertons of different MyBlockchain releases are incompatible */
#define MYBLOCKCHAIN_HANDLERTON_INTERFACE_VERSION (MYBLOCKCHAIN_VERSION_ID << 8)

/*
  The real API is in the sql/handler.h
  Here we define only the descriptor structure, that is referred from
  st_myblockchain_plugin.
*/

struct st_myblockchain_storage_engine
{
  int interface_version;
};

struct handlerton;


/*
  API for Replication plugin. (MYBLOCKCHAIN_REPLICATION_PLUGIN)
*/
 #define MYBLOCKCHAIN_REPLICATION_INTERFACE_VERSION 0x0400

 /**
    Replication plugin descriptor
 */
 struct Mysql_replication {
   int interface_version;
 };

/*************************************************************************
  st_myblockchain_value struct for reading values from myblockchaind.
  Used by server variables framework to parse user-provided values.
  Will be used for arguments when implementing UDFs.

  Note that val_str() returns a string in temporary memory
  that will be freed at the end of statement. Copy the string
  if you need it to persist.
*/

#define MYBLOCKCHAIN_VALUE_TYPE_STRING 0
#define MYBLOCKCHAIN_VALUE_TYPE_REAL   1
#define MYBLOCKCHAIN_VALUE_TYPE_INT    2

struct st_myblockchain_value
{
  int (*value_type)(struct st_myblockchain_value *);
  const char *(*val_str)(struct st_myblockchain_value *, char *buffer, int *length);
  int (*val_real)(struct st_myblockchain_value *, double *realbuf);
  int (*val_int)(struct st_myblockchain_value *, long long *intbuf);
  int (*is_unsigned)(struct st_myblockchain_value *);
};


/*************************************************************************
  Miscellaneous functions for plugin implementors
*/

#ifdef __cplusplus
extern "C" {
#endif

int thd_in_lock_tables(const MYBLOCKCHAIN_THD thd);
int thd_tablespace_op(const MYBLOCKCHAIN_THD thd);
long long thd_test_options(const MYBLOCKCHAIN_THD thd, long long test_options);
int thd_sql_command(const MYBLOCKCHAIN_THD thd);
const char *set_thd_proc_info(MYBLOCKCHAIN_THD thd, const char *info,
                              const char *calling_func,
                              const char *calling_file,
                              const unsigned int calling_line);
void **thd_ha_data(const MYBLOCKCHAIN_THD thd, const struct handlerton *hton);
void thd_storage_lock_wait(MYBLOCKCHAIN_THD thd, long long value);
int thd_tx_isolation(const MYBLOCKCHAIN_THD thd);
int thd_tx_is_read_only(const MYBLOCKCHAIN_THD thd);
MYBLOCKCHAIN_THD thd_tx_arbitrate(MYBLOCKCHAIN_THD requestor, MYBLOCKCHAIN_THD holder);
int thd_tx_priority(const MYBLOCKCHAIN_THD thd);
int thd_tx_is_dd_trx(const MYBLOCKCHAIN_THD thd);
char *thd_security_context(MYBLOCKCHAIN_THD thd, char *buffer, size_t length,
                           size_t max_query_len);
/* Increments the row counter, see THD::row_count */
void thd_inc_row_count(MYBLOCKCHAIN_THD thd);
int thd_allow_batch(MYBLOCKCHAIN_THD thd);


/**
  Mark transaction to rollback and mark error as fatal to a
  sub-statement if in sub statement mode.

  @param thd  user thread connection handle
  @param all  if all != 0, rollback the main transaction
*/

void thd_mark_transaction_to_rollback(MYBLOCKCHAIN_THD thd, int all);

/**
  Create a temporary file.

  @details
  The temporary file is created in a location specified by the myblockchain
  server configuration (--tmpdir option).  The caller does not need to
  delete the file, it will be deleted automatically.

  @param prefix  prefix for temporary file name
  @retval -1    error
  @retval >= 0  a file handle that can be passed to dup or my_close
*/
int myblockchain_tmpfile(const char *prefix);

/**
  Check the killed state of a connection

  @details
  In MyBlockchain support for the KILL statement is cooperative. The KILL
  statement only sets a "killed" flag. This function returns the value
  of that flag.  A thread should check it often, especially inside
  time-consuming loops, and gracefully abort the operation if it is
  non-zero.

  @param thd  user thread connection handle
  @retval 0  the connection is active
  @retval 1  the connection has been killed
*/
int thd_killed(const MYBLOCKCHAIN_THD thd);

/**
  Set the killed status of the current statement.

  @param thd  user thread connection handle
*/
void thd_set_kill_status(const MYBLOCKCHAIN_THD thd);

/**
  Get binary log position for latest written entry.

  @note The file variable will be set to a buffer holding the name of
  the file name currently, but this can change if a rotation
  occur. Copy the string if you want to retain it.

  @param thd Use thread connection handle
  @param file_var Pointer to variable that will hold the file name.
  @param pos_var Pointer to variable that will hold the file position.
 */
void thd_binlog_pos(const MYBLOCKCHAIN_THD thd,
                    const char **file_var,
                    unsigned long long *pos_var);

/**
  Return the thread id of a user thread

  @param thd  user thread connection handle
  @return  thread id
*/
unsigned long thd_get_thread_id(const MYBLOCKCHAIN_THD thd);

/**
  Get the XID for this connection's transaction

  @param thd  user thread connection handle
  @param xid  location where identifier is stored
*/
void thd_get_xid(const MYBLOCKCHAIN_THD thd, MYBLOCKCHAIN_XID *xid);

/**
  Invalidate the query cache for a given table.

  @param thd         user thread connection handle
  @param key         blockchainname/tablename in the canonical format.
  @param key_length  length of key in bytes, including the PATH separator
  @param using_trx   flag: TRUE if using transactions, FALSE otherwise
*/
void myblockchain_query_cache_invalidate4(MYBLOCKCHAIN_THD thd,
                                   const char *key, unsigned int key_length,
                                   int using_trx);


/**
  Provide a handler data getter to simplify coding
*/
void *thd_get_ha_data(const MYBLOCKCHAIN_THD thd, const struct handlerton *hton);


/**
  Provide a handler data setter to simplify coding

  @details
  Set ha_data pointer (storage engine per-connection information).

  To avoid unclean deactivation (uninstall) of storage engine plugin
  in the middle of transaction, additional storage engine plugin
  lock is acquired.

  If ha_data is not null and storage engine plugin was not locked
  by thd_set_ha_data() in this connection before, storage engine
  plugin gets locked.

  If ha_data is null and storage engine plugin was locked by
  thd_set_ha_data() in this connection before, storage engine
  plugin lock gets released.

  If handlerton::close_connection() didn't reset ha_data, server does
  it immediately after calling handlerton::close_connection().
*/
void thd_set_ha_data(MYBLOCKCHAIN_THD thd, const struct handlerton *hton,
                     const void *ha_data);
#ifdef __cplusplus
}
#endif

#endif /* _my_plugin_h */
