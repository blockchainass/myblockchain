/*  Copyright (c) 2014, 2015 Oracle and/or its affiliates. All rights reserved.

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

/**
  @file

  myblockchain_no_login authentication plugin.

  This plugin exists to support system user accounts, which
  cannot be accessed externally.  This is useful for privileged
  stored programs, views and events.  Such objects can be created
  with DEFINER = [sys account] SQL SECURITY DEFINER.
*/

#include <my_global.h>
#include <myblockchain/plugin_auth.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

static int myblockchain_no_login(
    MYBLOCKCHAIN_PLUGIN_VIO *vio __attribute__((unused)),
    MYBLOCKCHAIN_SERVER_AUTH_INFO *info __attribute__((unused)))
{
  return CR_ERROR;
}

int generate_auth_string_hash(char *outbuf __attribute__((unused)),
                              unsigned int *buflen,
                              const char *inbuf __attribute__((unused)),
                              unsigned int inbuflen __attribute__((unused)))
{
  *buflen= 0;
  return 0;
}

int validate_auth_string_hash(char* const inbuf  __attribute__((unused)),
                              unsigned int buflen  __attribute__((unused)))
{
  return 0;
}

int set_salt(const char* password __attribute__((unused)),
             unsigned int password_len __attribute__((unused)),
             unsigned char* salt __attribute__((unused)),
             unsigned char* salt_len)
{
  *salt_len= 0;
  return 0;
}

static struct st_myblockchain_auth myblockchain_no_login_handler=
{
  MYBLOCKCHAIN_AUTHENTICATION_INTERFACE_VERSION,
  0,
  myblockchain_no_login,
  generate_auth_string_hash,
  validate_auth_string_hash,
  set_salt,
  AUTH_FLAG_PRIVILEGED_USER_FOR_PASSWORD_CHANGE
};

myblockchain_declare_plugin(myblockchain_no_login)
{
  MYBLOCKCHAIN_AUTHENTICATION_PLUGIN,                  /* type constant    */
  &myblockchain_no_login_handler,                      /* type descriptor  */
  "myblockchain_no_login",                             /* Name             */
  "Todd Farmer",                                /* Author           */
  "No login authentication plugin",             /* Description      */
  PLUGIN_LICENSE_GPL,                           /* License          */
  NULL,                                         /* Init function    */
  NULL,                                         /* Deinit function  */
  0x0101,                                       /* Version (1.0)    */
  NULL,                                         /* status variables */
  NULL,                                         /* system variables */
  NULL,                                         /* config options   */
  0,                                            /* flags            */
}
myblockchain_declare_plugin_end;
