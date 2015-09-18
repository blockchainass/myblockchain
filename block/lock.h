/* Copyright (c) 2010, 2015, Oracle and/or its affiliates. All rights reserved.

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

#ifndef LOCK_INCLUDED
#define LOCK_INCLUDED

#include "thr_lock.h"                           /* thr_lock_type */
#include "mdl.h"

// Forward declarations
struct TABLE;
struct TABLE_LIST;
class THD;
typedef struct st_myblockchain_lock MYBLOCKCHAIN_LOCK;


MYBLOCKCHAIN_LOCK *myblockchain_lock_tables(THD *thd, TABLE **table, size_t count, uint flags);
void myblockchain_unlock_tables(THD *thd, MYBLOCKCHAIN_LOCK *sql_lock);
void myblockchain_unlock_read_tables(THD *thd, MYBLOCKCHAIN_LOCK *sql_lock);
void myblockchain_unlock_some_tables(THD *thd, TABLE **table,uint count);
void myblockchain_lock_remove(THD *thd, MYBLOCKCHAIN_LOCK *locked,TABLE *table);
void myblockchain_lock_abort(THD *thd, TABLE *table, bool upgrade_lock);
void myblockchain_lock_abort_for_thread(THD *thd, TABLE *table);
MYBLOCKCHAIN_LOCK *myblockchain_lock_merge(MYBLOCKCHAIN_LOCK *a,MYBLOCKCHAIN_LOCK *b);
/* Lock based on name */
bool lock_schema_name(THD *thd, const char *db);
/* Lock based on tablespace name */
bool lock_tablespace_name(THD *thd, const char *tablespace);
/* Lock based on stored routine name */
bool lock_object_name(THD *thd, MDL_key::enum_mdl_namespace mdl_type,
                      const char *db, const char *name);

#endif /* LOCK_INCLUDED */
