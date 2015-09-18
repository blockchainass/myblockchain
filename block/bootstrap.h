/* Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved.

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

#ifndef BOOTSTRAP_H
#define BOOTSTRAP_H

#include "my_global.h"

typedef struct st_myblockchain_file MYBLOCKCHAIN_FILE;

/**
  Execute all commands from a file. Used by the myblockchain_install_db script to
  create MyBlockchain privilege tables without having to start a full MyBlockchain server.
*/
int bootstrap(MYBLOCKCHAIN_FILE *file);

#endif // BOOTSTRAP_H
