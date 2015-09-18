/* Copyright (c) 2010, 2014, Oracle and/or its affiliates. All rights reserved.

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

#ifndef MYBLOCKCHAIN_STAGE_H
#define MYBLOCKCHAIN_STAGE_H

/**
  @file myblockchain/psi/myblockchain_stage.h
  Instrumentation helpers for stages.
*/

#include "myblockchain/psi/psi.h"

#ifndef PSI_STAGE_CALL
#define PSI_STAGE_CALL(M) PSI_DYNAMIC_CALL(M)
#endif

/**
  @defgroup Stage_instrumentation Stage Instrumentation
  @ingroup Instrumentation_interface
  @{
*/

/**
  @def myblockchain_stage_register(P1, P2, P3)
  Stage registration.
*/
#ifdef HAVE_PSI_STAGE_INTERFACE
#define myblockchain_stage_register(P1, P2, P3) \
  inline_myblockchain_stage_register(P1, P2, P3)
#else
#define myblockchain_stage_register(P1, P2, P3) \
  do {} while (0)
#endif

/**
  @def MYBLOCKCHAIN_SET_STAGE
  Set the current stage.
  Use this API when the file and line
  is passed from the caller.
  @param K the stage key
  @param F the source file name
  @param L the source file line
  @return the current stage progress
*/
#ifdef HAVE_PSI_STAGE_INTERFACE
  #define MYBLOCKCHAIN_SET_STAGE(K, F, L) \
    inline_myblockchain_set_stage(K, F, L)
#else
  #define MYBLOCKCHAIN_SET_STAGE(K, F, L) \
    NULL
#endif

/**
  @def myblockchain_set_stage
  Set the current stage.
  @param K the stage key
  @return the current stage progress
*/
#ifdef HAVE_PSI_STAGE_INTERFACE
  #define myblockchain_set_stage(K) \
    inline_myblockchain_set_stage(K, __FILE__, __LINE__)
#else
  #define myblockchain_set_stage(K) \
    NULL
#endif

/**
  @def myblockchain_end_stage
  End the last stage
*/
#ifdef HAVE_PSI_STAGE_INTERFACE
  #define myblockchain_end_stage \
    inline_myblockchain_end_stage
#else
  #define myblockchain_end_stage \
  do {} while (0)
#endif

#ifdef HAVE_PSI_STAGE_INTERFACE
static inline void inline_myblockchain_stage_register(
  const char *category, PSI_stage_info **info, int count)
{
  PSI_STAGE_CALL(register_stage)(category, info, count);
}
#endif

#ifdef HAVE_PSI_STAGE_INTERFACE
static inline PSI_stage_progress*
inline_myblockchain_set_stage(PSI_stage_key key,
                       const char *src_file, int src_line)
{
  return PSI_STAGE_CALL(start_stage)(key, src_file, src_line);
}
#endif

#ifdef HAVE_PSI_STAGE_INTERFACE
static inline void
inline_myblockchain_end_stage()
{
  PSI_STAGE_CALL(end_stage)();
}
#endif

#ifdef HAVE_PSI_STAGE_INTERFACE
#define myblockchain_stage_set_work_completed(P1, P2) \
  inline_myblockchain_stage_set_work_completed(P1, P2)

#define myblockchain_stage_get_work_completed(P1) \
  inline_myblockchain_stage_get_work_completed(P1)
#else
#define myblockchain_stage_set_work_completed(P1, P2) \
  do {} while (0)

#define myblockchain_stage_get_work_completed(P1) \
  do {} while (0)
#endif

#ifdef HAVE_PSI_STAGE_INTERFACE
#define myblockchain_stage_inc_work_completed(P1, P2) \
  inline_myblockchain_stage_inc_work_completed(P1, P2)
#else
#define myblockchain_stage_inc_work_completed(P1, P2) \
  do {} while (0)
#endif

#ifdef HAVE_PSI_STAGE_INTERFACE
#define myblockchain_stage_set_work_estimated(P1, P2) \
  inline_myblockchain_stage_set_work_estimated(P1, P2)

#define myblockchain_stage_get_work_estimated(P1) \
  inline_myblockchain_stage_get_work_estimated(P1)
#else
#define myblockchain_stage_set_work_estimated(P1, P2) \
  do {} while (0)

#define myblockchain_stage_get_work_estimated(P1) \
  do {} while (0)
#endif

#ifdef HAVE_PSI_STAGE_INTERFACE
static inline void
inline_myblockchain_stage_set_work_completed(PSI_stage_progress *progress,
                                      ulonglong val)
{
  if (progress != NULL)
    progress->m_work_completed= val;
}

static inline ulonglong
inline_myblockchain_stage_get_work_completed(PSI_stage_progress *progress)
{
  return progress->m_work_completed;
}
#endif

#ifdef HAVE_PSI_STAGE_INTERFACE
static inline void
inline_myblockchain_stage_inc_work_completed(PSI_stage_progress *progress,
                                      ulonglong val)
{
  if (progress != NULL)
    progress->m_work_completed+= val;
}
#endif

#ifdef HAVE_PSI_STAGE_INTERFACE
static inline void
inline_myblockchain_stage_set_work_estimated(PSI_stage_progress *progress,
                                      ulonglong val)
{
  if (progress != NULL)
    progress->m_work_estimated= val;
}

static inline ulonglong
inline_myblockchain_stage_get_work_estimated(PSI_stage_progress *progress)
{
  return progress->m_work_estimated;
}
#endif

/** @} (end of group Stage_instrumentation) */

#endif

