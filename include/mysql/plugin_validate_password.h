/* Copyright (c) 2012, 2014, Oracle and/or its affiliates. All rights reserved.

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

#ifndef MYBLOCKCHAIN_PLUGIN_VALIDATE_PASSWORD_INCLUDED
#define MYBLOCKCHAIN_PLUGIN_VALIDATE_PASSWORD_INCLUDED

/* API for validate_password plugin. (MYBLOCKCHAIN_VALIDATE_PASSWORD_PLUGIN) */

#include <myblockchain/plugin.h>
#define MYBLOCKCHAIN_VALIDATE_PASSWORD_INTERFACE_VERSION 0x0100

/*  
  The descriptor structure for the plugin, that is referred from
  st_myblockchain_plugin.
*/

typedef void* myblockchain_string_handle;

struct st_myblockchain_validate_password
{
  int interface_version;
  /*  
    This function retuns TRUE for passwords which satisfy the password
    policy (as choosen by plugin variable) and FALSE for all other
    password
  */
  int (*validate_password)(myblockchain_string_handle password);
  /*  
    This function returns the password strength (0-100) depending
    upon the policies
  */
  int (*get_password_strength)(myblockchain_string_handle password);
};
#endif
