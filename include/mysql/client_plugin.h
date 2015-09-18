#ifndef MYBLOCKCHAIN_CLIENT_PLUGIN_INCLUDED
/* Copyright (c) 2010, 2013, Oracle and/or its affiliates. All rights reserved.

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

/**
  @file

  MyBlockchain Client Plugin API

  This file defines the API for plugins that work on the client side
*/
#define MYBLOCKCHAIN_CLIENT_PLUGIN_INCLUDED

#ifndef MYBLOCKCHAIN_ABI_CHECK
#include <stdarg.h>
#include <stdlib.h>
#endif

/*
  On Windows, exports from DLL need to be declared.
  Also, plugin needs to be declared as extern "C" because MSVC
  unlike other compilers, uses C++ mangling for variables not only
  for functions.
*/

#undef MYBLOCKCHAIN_PLUGIN_EXPORT

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
extern "C" {
#endif

/* known plugin types */
#define MYBLOCKCHAIN_CLIENT_reserved1               0
#define MYBLOCKCHAIN_CLIENT_reserved2               1
#define MYBLOCKCHAIN_CLIENT_AUTHENTICATION_PLUGIN   2
#define MYBLOCKCHAIN_CLIENT_TRACE_PLUGIN            3

#define MYBLOCKCHAIN_CLIENT_AUTHENTICATION_PLUGIN_INTERFACE_VERSION  0x0100
#define MYBLOCKCHAIN_CLIENT_TRACE_PLUGIN_INTERFACE_VERSION           0x0100

#define MYBLOCKCHAIN_CLIENT_MAX_PLUGINS             4

#define myblockchain_declare_client_plugin(X)          \
     MYBLOCKCHAIN_PLUGIN_EXPORT struct st_myblockchain_client_plugin_ ## X        \
        _myblockchain_client_plugin_declaration_ = {   \
          MYBLOCKCHAIN_CLIENT_ ## X ## _PLUGIN,        \
          MYBLOCKCHAIN_CLIENT_ ## X ## _PLUGIN_INTERFACE_VERSION,
#define myblockchain_end_client_plugin             }

/* generic plugin header structure */
#define MYBLOCKCHAIN_CLIENT_PLUGIN_HEADER                      \
  int type;                                             \
  unsigned int interface_version;                       \
  const char *name;                                     \
  const char *author;                                   \
  const char *desc;                                     \
  unsigned int version[3];                              \
  const char *license;                                  \
  void *myblockchain_api;                                      \
  int (*init)(char *, size_t, int, va_list);            \
  int (*deinit)();                                      \
  int (*options)(const char *option, const void *);

struct st_myblockchain_client_plugin
{
  MYBLOCKCHAIN_CLIENT_PLUGIN_HEADER
};

struct st_myblockchain;

/******** authentication plugin specific declarations *********/
#include <myblockchain/plugin_auth_common.h>

struct st_myblockchain_client_plugin_AUTHENTICATION
{
  MYBLOCKCHAIN_CLIENT_PLUGIN_HEADER
  int (*authenticate_user)(MYBLOCKCHAIN_PLUGIN_VIO *vio, struct st_myblockchain *myblockchain);
};

/******** using plugins ************/

/**
  loads a plugin and initializes it

  @param myblockchain  MYBLOCKCHAIN structure.
  @param name   a name of the plugin to load
  @param type   type of plugin that should be loaded, -1 to disable type check
  @param argc   number of arguments to pass to the plugin initialization
                function
  @param ...    arguments for the plugin initialization function

  @retval
  a pointer to the loaded plugin, or NULL in case of a failure
*/
struct st_myblockchain_client_plugin *
myblockchain_load_plugin(struct st_myblockchain *myblockchain, const char *name, int type,
                  int argc, ...);

/**
  loads a plugin and initializes it, taking va_list as an argument

  This is the same as myblockchain_load_plugin, but take va_list instead of
  a list of arguments.

  @param myblockchain  MYBLOCKCHAIN structure.
  @param name   a name of the plugin to load
  @param type   type of plugin that should be loaded, -1 to disable type check
  @param argc   number of arguments to pass to the plugin initialization
                function
  @param args   arguments for the plugin initialization function

  @retval
  a pointer to the loaded plugin, or NULL in case of a failure
*/
struct st_myblockchain_client_plugin *
myblockchain_load_plugin_v(struct st_myblockchain *myblockchain, const char *name, int type,
                    int argc, va_list args);

/**
  finds an already loaded plugin by name, or loads it, if necessary

  @param myblockchain  MYBLOCKCHAIN structure.
  @param name   a name of the plugin to load
  @param type   type of plugin that should be loaded

  @retval
  a pointer to the plugin, or NULL in case of a failure
*/
struct st_myblockchain_client_plugin *
myblockchain_client_find_plugin(struct st_myblockchain *myblockchain, const char *name, int type);

/**
  adds a plugin structure to the list of loaded plugins

  This is useful if an application has the necessary functionality
  (for example, a special load data handler) statically linked into
  the application binary. It can use this function to register the plugin
  directly, avoiding the need to factor it out into a shared object.

  @param myblockchain  MYBLOCKCHAIN structure. It is only used for error reporting
  @param plugin an st_myblockchain_client_plugin structure to register

  @retval
  a pointer to the plugin, or NULL in case of a failure
*/
struct st_myblockchain_client_plugin *
myblockchain_client_register_plugin(struct st_myblockchain *myblockchain,
                             struct st_myblockchain_client_plugin *plugin);

/**
  set plugin options

  Can be used to set extra options and affect behavior for a plugin.
  This function may be called multiple times to set several options

  @param plugin an st_myblockchain_client_plugin structure
  @param option a string which specifies the option to set
  @param value  value for the option.

  @retval 0 on success, 1 in case of failure
**/
int myblockchain_plugin_options(struct st_myblockchain_client_plugin *plugin,
                         const char *option, const void *value);


#ifdef __cplusplus
}
#endif

#endif

