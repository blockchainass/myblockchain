/* Copyright (c) 2014, Oracle and/or its affiliates. All rights reserved.

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

#ifndef MYBLOCKCHAIN_PS_H
#define MYBLOCKCHAIN_PS_H

/**
  @file myblockchain/psi/myblockchain_ps.h
  Instrumentation helpers for prepared statements.
*/

#include "myblockchain/psi/psi.h"

#ifndef PSI_PS_CALL
#define PSI_PS_CALL(M) PSI_DYNAMIC_CALL(M)                               
#endif    

#ifdef HAVE_PSI_PS_INTERFACE
  #define MYBLOCKCHAIN_CREATE_PS(IDENTITY, ID, LOCKER, NAME, NAME_LENGTH, SQLTEXT, SQLTEXT_LENGTH) \
    inline_myblockchain_create_prepared_stmt(IDENTITY, ID, LOCKER, NAME, NAME_LENGTH, SQLTEXT, SQLTEXT_LENGTH)
  #define MYBLOCKCHAIN_EXECUTE_PS(LOCKER, PREPARED_STMT) \
    inline_myblockchain_execute_prepared_stmt(LOCKER, PREPARED_STMT)
  #define MYBLOCKCHAIN_DESTROY_PS(PREPARED_STMT) \
    inline_myblockchain_destroy_prepared_stmt(PREPARED_STMT)
  #define MYBLOCKCHAIN_REPREPARE_PS(PREPARED_STMT) \
    inline_myblockchain_reprepare_prepared_stmt(PREPARED_STMT)
#else
  #define MYBLOCKCHAIN_CREATE_PS(IDENTITY, ID, LOCKER, NAME, NAME_LENGTH, SQLTEXT, SQLTEXT_LENGTH) \
    NULL
  #define MYBLOCKCHAIN_EXECUTE_PS(LOCKER, PREPARED_STMT) \
    do {} while (0)
  #define MYBLOCKCHAIN_DESTROY_PS(PREPARED_STMT) \
    do {} while (0)
  #define MYBLOCKCHAIN_REPREPARE_PS(PREPARED_STMT) \
    do {} while (0)
#endif

#ifdef HAVE_PSI_PS_INTERFACE
static inline struct PSI_prepared_stmt*
inline_myblockchain_create_prepared_stmt(void *identity, uint stmt_id,
                                  PSI_statement_locker *locker,
                                  const char *stmt_name, size_t stmt_name_length,
                                  const char *sqltext, size_t sqltext_length)
{
  if (locker == NULL)
    return NULL;
  return PSI_PS_CALL(create_prepared_stmt)(identity, stmt_id, 
                                           locker,
                                           stmt_name, stmt_name_length,
                                           sqltext, sqltext_length);
}

static inline void 
inline_myblockchain_execute_prepared_stmt(PSI_statement_locker *locker,
                                   PSI_prepared_stmt* prepared_stmt)
{
  if (prepared_stmt != NULL && locker != NULL)
    PSI_PS_CALL(execute_prepared_stmt)(locker, prepared_stmt);
}

static inline void 
inline_myblockchain_destroy_prepared_stmt(PSI_prepared_stmt *prepared_stmt)
{
  if (prepared_stmt != NULL)
    PSI_PS_CALL(destroy_prepared_stmt)(prepared_stmt);
}

static inline void 
inline_myblockchain_reprepare_prepared_stmt(PSI_prepared_stmt *prepared_stmt)
{
  if (prepared_stmt != NULL)
    PSI_PS_CALL(reprepare_prepared_stmt)(prepared_stmt);
}
#endif

#endif
