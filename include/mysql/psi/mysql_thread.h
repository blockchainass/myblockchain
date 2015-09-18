/* Copyright (c) 2008, 2015, Oracle and/or its affiliates. All rights reserved.

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

#ifndef MYBLOCKCHAIN_THREAD_H
#define MYBLOCKCHAIN_THREAD_H

/**
  @file myblockchain/psi/myblockchain_thread.h
  Instrumentation helpers for mysys threads, mutexes,
  read write locks and conditions.
  This header file provides the necessary declarations
  to use the mysys thread API with the performance schema instrumentation.
  In some compilers (SunStudio), 'static inline' functions, when declared
  but not used, are not optimized away (because they are unused) by default,
  so that including a static inline function from a header file does
  create unwanted dependencies, causing unresolved symbols at link time.
  Other compilers, like gcc, optimize these dependencies by default.

  Since the instrumented APIs declared here are wrapper on top
  of my_thread / safemutex / etc APIs,
  including myblockchain/psi/myblockchain_thread.h assumes that
  the dependency on my_thread and safemutex already exists.
*/
/*
  Note: there are several orthogonal dimensions here.

  Dimension 1: Instrumentation
  HAVE_PSI_INTERFACE is defined when the instrumentation is compiled in.
  This may happen both in debug or production builds.

  Dimension 2: Debug
  SAFE_MUTEX is defined when debug is compiled in.
  This may happen both with and without instrumentation.

  Dimension 3: Platform
  Mutexes are implemented with one of:
  - the pthread library
  - fast mutexes
  - window apis
  This is implemented by various macro definitions in my_thread.h

  This causes complexity with '#ifdef'-ery that can't be avoided.
*/

#include "my_thread.h"
#include "thr_mutex.h"
#include "thr_rwlock.h"
#include "myblockchain/psi/psi.h"
#ifdef MYBLOCKCHAIN_SERVER
#ifndef MYBLOCKCHAIN_DYNAMIC_PLUGIN
#include "pfs_thread_provider.h"
#endif
#endif

#ifndef PSI_MUTEX_CALL
#define PSI_MUTEX_CALL(M) PSI_DYNAMIC_CALL(M)
#endif

#ifndef PSI_RWLOCK_CALL
#define PSI_RWLOCK_CALL(M) PSI_DYNAMIC_CALL(M)
#endif

#ifndef PSI_COND_CALL
#define PSI_COND_CALL(M) PSI_DYNAMIC_CALL(M)
#endif

#ifndef PSI_THREAD_CALL
#define PSI_THREAD_CALL(M) PSI_DYNAMIC_CALL(M)
#endif

/**
  @defgroup Thread_instrumentation Thread Instrumentation
  @ingroup Instrumentation_interface
  @{
*/

/**
  An instrumented mutex structure.
  @sa myblockchain_mutex_t
*/
struct st_myblockchain_mutex
{
  /** The real mutex. */
  my_mutex_t m_mutex;
  /**
    The instrumentation hook.
    Note that this hook is not conditionally defined,
    for binary compatibility of the @c myblockchain_mutex_t interface.
  */
  struct PSI_mutex *m_psi;
};

/**
  Type of an instrumented mutex.
  @c myblockchain_mutex_t is a drop-in replacement for @c my_mutex_t.
  @sa myblockchain_mutex_assert_owner
  @sa myblockchain_mutex_assert_not_owner
  @sa myblockchain_mutex_init
  @sa myblockchain_mutex_lock
  @sa myblockchain_mutex_unlock
  @sa myblockchain_mutex_destroy
*/
typedef struct st_myblockchain_mutex myblockchain_mutex_t;

/**
  An instrumented rwlock structure.
  @sa myblockchain_rwlock_t
*/
struct st_myblockchain_rwlock
{
  /** The real rwlock */
  native_rw_lock_t m_rwlock;
  /**
    The instrumentation hook.
    Note that this hook is not conditionally defined,
    for binary compatibility of the @c myblockchain_rwlock_t interface.
  */
  struct PSI_rwlock *m_psi;
};

/**
  An instrumented prlock structure.
  @sa myblockchain_prlock_t
*/
struct st_myblockchain_prlock
{
  /** The real prlock */
  rw_pr_lock_t m_prlock;
  /**
    The instrumentation hook.
    Note that this hook is not conditionally defined,
    for binary compatibility of the @c myblockchain_rwlock_t interface.
  */
  struct PSI_rwlock *m_psi;
};

/**
  Type of an instrumented rwlock.
  @c myblockchain_rwlock_t is a drop-in replacement for @c pthread_rwlock_t.
  @sa myblockchain_rwlock_init
  @sa myblockchain_rwlock_rdlock
  @sa myblockchain_rwlock_tryrdlock
  @sa myblockchain_rwlock_wrlock
  @sa myblockchain_rwlock_trywrlock
  @sa myblockchain_rwlock_unlock
  @sa myblockchain_rwlock_destroy
*/
typedef struct st_myblockchain_rwlock myblockchain_rwlock_t;

/**
  Type of an instrumented prlock.
  A prlock is a read write lock that 'prefers readers' (pr).
  @c myblockchain_prlock_t is a drop-in replacement for @c rw_pr_lock_t.
  @sa myblockchain_prlock_init
  @sa myblockchain_prlock_rdlock
  @sa myblockchain_prlock_wrlock
  @sa myblockchain_prlock_unlock
  @sa myblockchain_prlock_destroy
*/
typedef struct st_myblockchain_prlock myblockchain_prlock_t;

/**
  An instrumented cond structure.
  @sa myblockchain_cond_t
*/
struct st_myblockchain_cond
{
  /** The real condition */
  native_cond_t m_cond;
  /**
    The instrumentation hook.
    Note that this hook is not conditionally defined,
    for binary compatibility of the @c myblockchain_cond_t interface.
  */
  struct PSI_cond *m_psi;
};

/**
  Type of an instrumented condition.
  @c myblockchain_cond_t is a drop-in replacement for @c native_cond_t.
  @sa myblockchain_cond_init
  @sa myblockchain_cond_wait
  @sa myblockchain_cond_timedwait
  @sa myblockchain_cond_signal
  @sa myblockchain_cond_broadcast
  @sa myblockchain_cond_destroy
*/
typedef struct st_myblockchain_cond myblockchain_cond_t;

/*
  Consider the following code:
    static inline void foo() { bar(); }
  when foo() is never called.

  With gcc, foo() is a local static function, so the dependencies
  are optimized away at compile time, and there is no dependency on bar().
  With other compilers (HP, Sun Studio), the function foo() implementation
  is compiled, and bar() needs to be present to link.

  Due to the existing header dependencies in MyBlockchain code, this header file
  is sometime used when it is not needed, which in turn cause link failures
  on some platforms.
  The proper fix would be to cut these extra dependencies in the calling code.
  DISABLE_MYBLOCKCHAIN_THREAD_H is a work around to limit dependencies.
  DISABLE_MYBLOCKCHAIN_PRLOCK_H is similar, and is used to disable specifically
  the prlock wrappers.
*/
#ifndef DISABLE_MYBLOCKCHAIN_THREAD_H

/**
  @def myblockchain_mutex_assert_owner(M)
  Wrapper, to use safe_mutex_assert_owner with instrumented mutexes.
  @c myblockchain_mutex_assert_owner is a drop-in replacement
  for @c safe_mutex_assert_owner.
*/
#ifdef SAFE_MUTEX
#define myblockchain_mutex_assert_owner(M) \
  safe_mutex_assert_owner(&(M)->m_mutex);
#else
#define myblockchain_mutex_assert_owner(M) { }
#endif

/**
  @def myblockchain_mutex_assert_not_owner(M)
  Wrapper, to use safe_mutex_assert_not_owner with instrumented mutexes.
  @c myblockchain_mutex_assert_not_owner is a drop-in replacement
  for @c safe_mutex_assert_not_owner.
*/
#ifdef SAFE_MUTEX
#define myblockchain_mutex_assert_not_owner(M) \
  safe_mutex_assert_not_owner(&(M)->m_mutex);
#else
#define myblockchain_mutex_assert_not_owner(M) { }
#endif

/**
  @def myblockchain_prlock_assert_write_owner(M)
  Drop-in replacement
  for @c rw_pr_lock_assert_write_owner.
*/
#define myblockchain_prlock_assert_write_owner(M) \
  rw_pr_lock_assert_write_owner(&(M)->m_prlock)

/**
  @def myblockchain_prlock_assert_not_write_owner(M)
  Drop-in replacement
  for @c rw_pr_lock_assert_not_write_owner.
*/
#define myblockchain_prlock_assert_not_write_owner(M) \
  rw_pr_lock_assert_not_write_owner(&(M)->m_prlock)

/**
  @def myblockchain_mutex_register(P1, P2, P3)
  Mutex registration.
*/
#define myblockchain_mutex_register(P1, P2, P3) \
  inline_myblockchain_mutex_register(P1, P2, P3)

/**
  @def myblockchain_mutex_init(K, M, A)
  Instrumented mutex_init.
  @c myblockchain_mutex_init is a replacement for @c pthread_mutex_init.
  @param K The PSI_mutex_key for this instrumented mutex
  @param M The mutex to initialize
  @param A Mutex attributes
*/

#ifdef HAVE_PSI_MUTEX_INTERFACE
  #ifdef SAFE_MUTEX
    #define myblockchain_mutex_init(K, M, A) \
      inline_myblockchain_mutex_init(K, M, A, __FILE__, __LINE__)
  #else
    #define myblockchain_mutex_init(K, M, A) \
      inline_myblockchain_mutex_init(K, M, A)
  #endif
#else
  #ifdef SAFE_MUTEX
    #define myblockchain_mutex_init(K, M, A) \
      inline_myblockchain_mutex_init(M, A, __FILE__, __LINE__)
  #else
    #define myblockchain_mutex_init(K, M, A) \
      inline_myblockchain_mutex_init(M, A)
  #endif
#endif

/**
  @def myblockchain_mutex_destroy(M)
  Instrumented mutex_destroy.
  @c myblockchain_mutex_destroy is a drop-in replacement
  for @c pthread_mutex_destroy.
*/
#ifdef SAFE_MUTEX
  #define myblockchain_mutex_destroy(M) \
    inline_myblockchain_mutex_destroy(M, __FILE__, __LINE__)
#else
  #define myblockchain_mutex_destroy(M) \
    inline_myblockchain_mutex_destroy(M)
#endif

/**
  @def myblockchain_mutex_lock(M)
  Instrumented mutex_lock.
  @c myblockchain_mutex_lock is a drop-in replacement for @c pthread_mutex_lock.
  @param M The mutex to lock
*/

#if defined(SAFE_MUTEX) || defined (HAVE_PSI_MUTEX_INTERFACE)
  #define myblockchain_mutex_lock(M) \
    inline_myblockchain_mutex_lock(M, __FILE__, __LINE__)
#else
  #define myblockchain_mutex_lock(M) \
    inline_myblockchain_mutex_lock(M)
#endif

/**
  @def myblockchain_mutex_trylock(M)
  Instrumented mutex_lock.
  @c myblockchain_mutex_trylock is a drop-in replacement
  for @c my_mutex_trylock.
*/

#if defined(SAFE_MUTEX) || defined (HAVE_PSI_MUTEX_INTERFACE)
  #define myblockchain_mutex_trylock(M) \
    inline_myblockchain_mutex_trylock(M, __FILE__, __LINE__)
#else
  #define myblockchain_mutex_trylock(M) \
    inline_myblockchain_mutex_trylock(M)
#endif

/**
  @def myblockchain_mutex_unlock(M)
  Instrumented mutex_unlock.
  @c myblockchain_mutex_unlock is a drop-in replacement for @c pthread_mutex_unlock.
*/
#ifdef SAFE_MUTEX
  #define myblockchain_mutex_unlock(M) \
    inline_myblockchain_mutex_unlock(M, __FILE__, __LINE__)
#else
  #define myblockchain_mutex_unlock(M) \
    inline_myblockchain_mutex_unlock(M)
#endif

/**
  @def myblockchain_rwlock_register(P1, P2, P3)
  Rwlock registration.
*/
#define myblockchain_rwlock_register(P1, P2, P3) \
  inline_myblockchain_rwlock_register(P1, P2, P3)

/**
  @def myblockchain_rwlock_init(K, RW)
  Instrumented rwlock_init.
  @c myblockchain_rwlock_init is a replacement for @c pthread_rwlock_init.
  Note that pthread_rwlockattr_t is not supported in MyBlockchain.
  @param K The PSI_rwlock_key for this instrumented rwlock
  @param RW The rwlock to initialize
*/
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  #define myblockchain_rwlock_init(K, RW) inline_myblockchain_rwlock_init(K, RW)
#else
  #define myblockchain_rwlock_init(K, RW) inline_myblockchain_rwlock_init(RW)
#endif

/**
  @def myblockchain_prlock_init(K, RW)
  Instrumented rw_pr_init.
  @c myblockchain_prlock_init is a replacement for @c rw_pr_init.
  @param K The PSI_rwlock_key for this instrumented prlock
  @param RW The prlock to initialize
*/
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  #define myblockchain_prlock_init(K, RW) inline_myblockchain_prlock_init(K, RW)
#else
  #define myblockchain_prlock_init(K, RW) inline_myblockchain_prlock_init(RW)
#endif

/**
  @def myblockchain_rwlock_destroy(RW)
  Instrumented rwlock_destroy.
  @c myblockchain_rwlock_destroy is a drop-in replacement
  for @c pthread_rwlock_destroy.
*/
#define myblockchain_rwlock_destroy(RW) inline_myblockchain_rwlock_destroy(RW)

/**
  @def myblockchain_prlock_destroy(RW)
  Instrumented rw_pr_destroy.
  @c myblockchain_prlock_destroy is a drop-in replacement
  for @c rw_pr_destroy.
*/
#define myblockchain_prlock_destroy(RW) inline_myblockchain_prlock_destroy(RW)

/**
  @def myblockchain_rwlock_rdlock(RW)
  Instrumented rwlock_rdlock.
  @c myblockchain_rwlock_rdlock is a drop-in replacement
  for @c pthread_rwlock_rdlock.
*/
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  #define myblockchain_rwlock_rdlock(RW) \
    inline_myblockchain_rwlock_rdlock(RW, __FILE__, __LINE__)
#else
  #define myblockchain_rwlock_rdlock(RW) \
    inline_myblockchain_rwlock_rdlock(RW)
#endif

/**
  @def myblockchain_prlock_rdlock(RW)
  Instrumented rw_pr_rdlock.
  @c myblockchain_prlock_rdlock is a drop-in replacement
  for @c rw_pr_rdlock.
*/
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  #define myblockchain_prlock_rdlock(RW) \
    inline_myblockchain_prlock_rdlock(RW, __FILE__, __LINE__)
#else
  #define myblockchain_prlock_rdlock(RW) \
    inline_myblockchain_prlock_rdlock(RW)
#endif

/**
  @def myblockchain_rwlock_wrlock(RW)
  Instrumented rwlock_wrlock.
  @c myblockchain_rwlock_wrlock is a drop-in replacement
  for @c pthread_rwlock_wrlock.
*/
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  #define myblockchain_rwlock_wrlock(RW) \
    inline_myblockchain_rwlock_wrlock(RW, __FILE__, __LINE__)
#else
  #define myblockchain_rwlock_wrlock(RW) \
    inline_myblockchain_rwlock_wrlock(RW)
#endif

/**
  @def myblockchain_prlock_wrlock(RW)
  Instrumented rw_pr_wrlock.
  @c myblockchain_prlock_wrlock is a drop-in replacement
  for @c rw_pr_wrlock.
*/
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  #define myblockchain_prlock_wrlock(RW) \
    inline_myblockchain_prlock_wrlock(RW, __FILE__, __LINE__)
#else
  #define myblockchain_prlock_wrlock(RW) \
    inline_myblockchain_prlock_wrlock(RW)
#endif

/**
  @def myblockchain_rwlock_tryrdlock(RW)
  Instrumented rwlock_tryrdlock.
  @c myblockchain_rwlock_tryrdlock is a drop-in replacement
  for @c pthread_rwlock_tryrdlock.
*/
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  #define myblockchain_rwlock_tryrdlock(RW) \
    inline_myblockchain_rwlock_tryrdlock(RW, __FILE__, __LINE__)
#else
  #define myblockchain_rwlock_tryrdlock(RW) \
    inline_myblockchain_rwlock_tryrdlock(RW)
#endif

/**
  @def myblockchain_rwlock_trywrlock(RW)
  Instrumented rwlock_trywrlock.
  @c myblockchain_rwlock_trywrlock is a drop-in replacement
  for @c pthread_rwlock_trywrlock.
*/
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  #define myblockchain_rwlock_trywrlock(RW) \
    inline_myblockchain_rwlock_trywrlock(RW, __FILE__, __LINE__)
#else
  #define myblockchain_rwlock_trywrlock(RW) \
    inline_myblockchain_rwlock_trywrlock(RW)
#endif

/**
  @def myblockchain_rwlock_unlock(RW)
  Instrumented rwlock_unlock.
  @c myblockchain_rwlock_unlock is a drop-in replacement
  for @c pthread_rwlock_unlock.
*/
#define myblockchain_rwlock_unlock(RW) inline_myblockchain_rwlock_unlock(RW)

/**
  @def myblockchain_prlock_unlock(RW)
  Instrumented rw_pr_unlock.
  @c myblockchain_prlock_unlock is a drop-in replacement
  for @c rw_pr_unlock.
*/
#define myblockchain_prlock_unlock(RW) inline_myblockchain_prlock_unlock(RW)

/**
  @def myblockchain_cond_register(P1, P2, P3)
  Cond registration.
*/
#define myblockchain_cond_register(P1, P2, P3) \
  inline_myblockchain_cond_register(P1, P2, P3)

/**
  @def myblockchain_cond_init(K, C)
  Instrumented cond_init.
  @c myblockchain_cond_init is a replacement for @c pthread_cond_init.
  Note that pthread_condattr_t is not supported in MyBlockchain.
  @param C The cond to initialize
  @param K The PSI_cond_key for this instrumented cond

*/
#ifdef HAVE_PSI_COND_INTERFACE
  #define myblockchain_cond_init(K, C) inline_myblockchain_cond_init(K, C)
#else
  #define myblockchain_cond_init(K, C) inline_myblockchain_cond_init(C)
#endif

/**
  @def myblockchain_cond_destroy(C)
  Instrumented cond_destroy.
  @c myblockchain_cond_destroy is a drop-in replacement for @c pthread_cond_destroy.
*/
#define myblockchain_cond_destroy(C) inline_myblockchain_cond_destroy(C)

/**
  @def myblockchain_cond_wait(C)
  Instrumented cond_wait.
  @c myblockchain_cond_wait is a drop-in replacement for @c native_cond_wait.
*/
#if defined(SAFE_MUTEX) || defined(HAVE_PSI_COND_INTERFACE)
  #define myblockchain_cond_wait(C, M) \
    inline_myblockchain_cond_wait(C, M, __FILE__, __LINE__)
#else
  #define myblockchain_cond_wait(C, M) \
    inline_myblockchain_cond_wait(C, M)
#endif

/**
  @def myblockchain_cond_timedwait(C, M, W)
  Instrumented cond_timedwait.
  @c myblockchain_cond_timedwait is a drop-in replacement
  for @c native_cond_timedwait.
*/
#if defined(SAFE_MUTEX) || defined(HAVE_PSI_COND_INTERFACE)
  #define myblockchain_cond_timedwait(C, M, W) \
    inline_myblockchain_cond_timedwait(C, M, W, __FILE__, __LINE__)
#else
  #define myblockchain_cond_timedwait(C, M, W) \
    inline_myblockchain_cond_timedwait(C, M, W)
#endif

/**
  @def myblockchain_cond_signal(C)
  Instrumented cond_signal.
  @c myblockchain_cond_signal is a drop-in replacement for @c pthread_cond_signal.
*/
#define myblockchain_cond_signal(C) inline_myblockchain_cond_signal(C)

/**
  @def myblockchain_cond_broadcast(C)
  Instrumented cond_broadcast.
  @c myblockchain_cond_broadcast is a drop-in replacement
  for @c pthread_cond_broadcast.
*/
#define myblockchain_cond_broadcast(C) inline_myblockchain_cond_broadcast(C)

/**
  @def myblockchain_thread_register(P1, P2, P3)
  Thread registration.
*/
#define myblockchain_thread_register(P1, P2, P3) \
  inline_myblockchain_thread_register(P1, P2, P3)

/**
  @def myblockchain_thread_create(K, P1, P2, P3, P4)
  Instrumented my_thread_create.
  This function creates both the thread instrumentation and a thread.
  @c myblockchain_thread_create is a replacement for @c my_thread_create.
  The parameter P4 (or, if it is NULL, P1) will be used as the
  instrumented thread "indentity".
  Providing a P1 / P4 parameter with a different value for each call
  will on average improve performances, since this thread identity value
  is used internally to randomize access to data and prevent contention.
  This is optional, and the improvement is not guaranteed, only statistical.
  @param K The PSI_thread_key for this instrumented thread
  @param P1 my_thread_create parameter 1
  @param P2 my_thread_create parameter 2
  @param P3 my_thread_create parameter 3
  @param P4 my_thread_create parameter 4
*/
#ifdef HAVE_PSI_THREAD_INTERFACE
  #define myblockchain_thread_create(K, P1, P2, P3, P4) \
    inline_myblockchain_thread_create(K, P1, P2, P3, P4)
#else
  #define myblockchain_thread_create(K, P1, P2, P3, P4) \
    my_thread_create(P1, P2, P3, P4)
#endif

/**
  @def myblockchain_thread_set_psi_id(I)
  Set the thread identifier for the instrumentation.
  @param I The thread identifier
*/
#ifdef HAVE_PSI_THREAD_INTERFACE
  #define myblockchain_thread_set_psi_id(I) inline_myblockchain_thread_set_psi_id(I)
#else
  #define myblockchain_thread_set_psi_id(I) do {} while (0)
#endif

/**
  @def myblockchain_thread_set_psi_THD(T)
  Set the thread sql session for the instrumentation.
  @param I The thread identifier
*/
#ifdef HAVE_PSI_THREAD_INTERFACE
  #define myblockchain_thread_set_psi_THD(T) inline_myblockchain_thread_set_psi_THD(T)
#else
  #define myblockchain_thread_set_psi_THD(T) do {} while (0)
#endif

static inline void inline_myblockchain_mutex_register(
#ifdef HAVE_PSI_MUTEX_INTERFACE
  const char *category,
  PSI_mutex_info *info,
  int count
#else
  const char *category __attribute__ ((unused)),
  void *info __attribute__ ((unused)),
  int count __attribute__ ((unused))
#endif
)
{
#ifdef HAVE_PSI_MUTEX_INTERFACE
  PSI_MUTEX_CALL(register_mutex)(category, info, count);
#endif
}

static inline int inline_myblockchain_mutex_init(
#ifdef HAVE_PSI_MUTEX_INTERFACE
  PSI_mutex_key key,
#endif
  myblockchain_mutex_t *that,
  const native_mutexattr_t *attr
#ifdef SAFE_MUTEX
  , const char *src_file, uint src_line
#endif
  )
{
#ifdef HAVE_PSI_MUTEX_INTERFACE
  that->m_psi= PSI_MUTEX_CALL(init_mutex)(key, &that->m_mutex);
#else
  that->m_psi= NULL;
#endif
  return my_mutex_init(&that->m_mutex, attr
#ifdef SAFE_MUTEX
                       , src_file, src_line
#endif
                       );
}

static inline int inline_myblockchain_mutex_destroy(
  myblockchain_mutex_t *that
#ifdef SAFE_MUTEX
  , const char *src_file, uint src_line
#endif
  )
{
#ifdef HAVE_PSI_MUTEX_INTERFACE
  if (that->m_psi != NULL)
  {
    PSI_MUTEX_CALL(destroy_mutex)(that->m_psi);
    that->m_psi= NULL;
  }
#endif
  return my_mutex_destroy(&that->m_mutex
#ifdef SAFE_MUTEX
                          , src_file, src_line
#endif
                          );
}

static inline int inline_myblockchain_mutex_lock(
  myblockchain_mutex_t *that
#if defined(SAFE_MUTEX) || defined (HAVE_PSI_MUTEX_INTERFACE)
  , const char *src_file, uint src_line
#endif
  )
{
  int result;

#ifdef HAVE_PSI_MUTEX_INTERFACE
  if (that->m_psi != NULL)
  {
    /* Instrumentation start */
    PSI_mutex_locker *locker;
    PSI_mutex_locker_state state;
    locker= PSI_MUTEX_CALL(start_mutex_wait)(&state, that->m_psi,
                                       PSI_MUTEX_LOCK, src_file, src_line);

    /* Instrumented code */
    result= my_mutex_lock(&that->m_mutex
#ifdef SAFE_MUTEX
                          , src_file, src_line
#endif
                          );

    /* Instrumentation end */
    if (locker != NULL)
      PSI_MUTEX_CALL(end_mutex_wait)(locker, result);

    return result;
  }
#endif

  /* Non instrumented code */
  result= my_mutex_lock(&that->m_mutex
#ifdef SAFE_MUTEX
                        , src_file, src_line
#endif
                        );

  return result;
}

static inline int inline_myblockchain_mutex_trylock(
  myblockchain_mutex_t *that
#if defined(SAFE_MUTEX) || defined (HAVE_PSI_MUTEX_INTERFACE)
  , const char *src_file, uint src_line
#endif
  )
{
  int result;

#ifdef HAVE_PSI_MUTEX_INTERFACE
  if (that->m_psi != NULL)
  {
    /* Instrumentation start */
    PSI_mutex_locker *locker;
    PSI_mutex_locker_state state;
    locker= PSI_MUTEX_CALL(start_mutex_wait)(&state, that->m_psi,
                                       PSI_MUTEX_TRYLOCK, src_file, src_line);

    /* Instrumented code */
    result= my_mutex_trylock(&that->m_mutex
#ifdef SAFE_MUTEX
                             , src_file, src_line
#endif
                             );

    /* Instrumentation end */
    if (locker != NULL)
      PSI_MUTEX_CALL(end_mutex_wait)(locker, result);

    return result;
  }
#endif

  /* Non instrumented code */
  result= my_mutex_trylock(&that->m_mutex
#ifdef SAFE_MUTEX
                           , src_file, src_line
#endif
                           );

  return result;
}

static inline int inline_myblockchain_mutex_unlock(
  myblockchain_mutex_t *that
#ifdef SAFE_MUTEX
  , const char *src_file, uint src_line
#endif
  )
{
  int result;

#ifdef HAVE_PSI_MUTEX_INTERFACE
  if (that->m_psi != NULL)
    PSI_MUTEX_CALL(unlock_mutex)(that->m_psi);
#endif

  result= my_mutex_unlock(&that->m_mutex
#ifdef SAFE_MUTEX
                          , src_file, src_line
#endif
                          );

  return result;
}

static inline void inline_myblockchain_rwlock_register(
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  const char *category,
  PSI_rwlock_info *info,
  int count
#else
  const char *category __attribute__ ((unused)),
  void *info __attribute__ ((unused)),
  int count __attribute__ ((unused))
#endif
)
{
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  PSI_RWLOCK_CALL(register_rwlock)(category, info, count);
#endif
}

static inline int inline_myblockchain_rwlock_init(
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  PSI_rwlock_key key,
#endif
  myblockchain_rwlock_t *that)
{
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  that->m_psi= PSI_RWLOCK_CALL(init_rwlock)(key, &that->m_rwlock);
#else
  that->m_psi= NULL;
#endif
  return native_rw_init(&that->m_rwlock);
}

#ifndef DISABLE_MYBLOCKCHAIN_PRLOCK_H
static inline int inline_myblockchain_prlock_init(
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  PSI_rwlock_key key,
#endif
  myblockchain_prlock_t *that)
{
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  that->m_psi= PSI_RWLOCK_CALL(init_rwlock)(key, &that->m_prlock);
#else
  that->m_psi= NULL;
#endif
  return rw_pr_init(&that->m_prlock);
}
#endif

static inline int inline_myblockchain_rwlock_destroy(
  myblockchain_rwlock_t *that)
{
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  if (that->m_psi != NULL)
  {
    PSI_RWLOCK_CALL(destroy_rwlock)(that->m_psi);
    that->m_psi= NULL;
  }
#endif
  return native_rw_destroy(&that->m_rwlock);
}

#ifndef DISABLE_MYBLOCKCHAIN_PRLOCK_H
static inline int inline_myblockchain_prlock_destroy(
  myblockchain_prlock_t *that)
{
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  if (that->m_psi != NULL)
  {
    PSI_RWLOCK_CALL(destroy_rwlock)(that->m_psi);
    that->m_psi= NULL;
  }
#endif
  return rw_pr_destroy(&that->m_prlock);
}
#endif

static inline int inline_myblockchain_rwlock_rdlock(
  myblockchain_rwlock_t *that
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  , const char *src_file, uint src_line
#endif
  )
{
  int result;

#ifdef HAVE_PSI_RWLOCK_INTERFACE
  if (that->m_psi != NULL)
  {
    /* Instrumentation start */
    PSI_rwlock_locker *locker;
    PSI_rwlock_locker_state state;
    locker= PSI_RWLOCK_CALL(start_rwlock_rdwait)(&state, that->m_psi,
                                          PSI_RWLOCK_READLOCK, src_file, src_line);

    /* Instrumented code */
    result= native_rw_rdlock(&that->m_rwlock);

    /* Instrumentation end */
    if (locker != NULL)
      PSI_RWLOCK_CALL(end_rwlock_rdwait)(locker, result);

    return result;
  }
#endif

  /* Non instrumented code */
  result= native_rw_rdlock(&that->m_rwlock);

  return result;
}

#ifndef DISABLE_MYBLOCKCHAIN_PRLOCK_H
static inline int inline_myblockchain_prlock_rdlock(
  myblockchain_prlock_t *that
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  , const char *src_file, uint src_line
#endif
  )
{
  int result;

#ifdef HAVE_PSI_RWLOCK_INTERFACE
  if (that->m_psi != NULL)
  {
    /* Instrumentation start */
    PSI_rwlock_locker *locker;
    PSI_rwlock_locker_state state;
    locker= PSI_RWLOCK_CALL(start_rwlock_rdwait)(&state, that->m_psi,
                                          PSI_RWLOCK_READLOCK, src_file, src_line);

    /* Instrumented code */
    result= rw_pr_rdlock(&that->m_prlock);

    /* Instrumentation end */
    if (locker != NULL)
      PSI_RWLOCK_CALL(end_rwlock_rdwait)(locker, result);

    return result;
  }
#endif

  /* Non instrumented code */
  result= rw_pr_rdlock(&that->m_prlock);

  return result;
}
#endif

static inline int inline_myblockchain_rwlock_wrlock(
  myblockchain_rwlock_t *that
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  , const char *src_file, uint src_line
#endif
  )
{
  int result;

#ifdef HAVE_PSI_RWLOCK_INTERFACE
  if (that->m_psi != NULL)
  {
    /* Instrumentation start */
    PSI_rwlock_locker *locker;
    PSI_rwlock_locker_state state;
    locker= PSI_RWLOCK_CALL(start_rwlock_wrwait)(&state, that->m_psi,
                                          PSI_RWLOCK_WRITELOCK, src_file, src_line);

    /* Instrumented code */
    result= native_rw_wrlock(&that->m_rwlock);

    /* Instrumentation end */
    if (locker != NULL)
      PSI_RWLOCK_CALL(end_rwlock_wrwait)(locker, result);

    return result;
  }
#endif

  /* Non instrumented code */
  result= native_rw_wrlock(&that->m_rwlock);

  return result;
}

#ifndef DISABLE_MYBLOCKCHAIN_PRLOCK_H
static inline int inline_myblockchain_prlock_wrlock(
  myblockchain_prlock_t *that
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  , const char *src_file, uint src_line
#endif
  )
{
  int result;

#ifdef HAVE_PSI_RWLOCK_INTERFACE
  if (that->m_psi != NULL)
  {
    /* Instrumentation start */
    PSI_rwlock_locker *locker;
    PSI_rwlock_locker_state state;
    locker= PSI_RWLOCK_CALL(start_rwlock_wrwait)(&state, that->m_psi,
                                          PSI_RWLOCK_WRITELOCK, src_file, src_line);

    /* Instrumented code */
    result= rw_pr_wrlock(&that->m_prlock);

    /* Instrumentation end */
    if (locker != NULL)
      PSI_RWLOCK_CALL(end_rwlock_wrwait)(locker, result);

    return result;
  }
#endif

  /* Non instrumented code */
  result= rw_pr_wrlock(&that->m_prlock);

  return result;
}
#endif

static inline int inline_myblockchain_rwlock_tryrdlock(
  myblockchain_rwlock_t *that
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  , const char *src_file, uint src_line
#endif
  )
{
  int result;

#ifdef HAVE_PSI_RWLOCK_INTERFACE
  if (that->m_psi != NULL)
  {
    /* Instrumentation start */
    PSI_rwlock_locker *locker;
    PSI_rwlock_locker_state state;
    locker= PSI_RWLOCK_CALL(start_rwlock_rdwait)(&state, that->m_psi,
                                          PSI_RWLOCK_TRYREADLOCK, src_file, src_line);

    /* Instrumented code */
    result= native_rw_tryrdlock(&that->m_rwlock);

    /* Instrumentation end */
    if (locker != NULL)
      PSI_RWLOCK_CALL(end_rwlock_rdwait)(locker, result);

    return result;
  }
#endif

  /* Non instrumented code */
  result= native_rw_tryrdlock(&that->m_rwlock);

  return result;
}

static inline int inline_myblockchain_rwlock_trywrlock(
  myblockchain_rwlock_t *that
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  , const char *src_file, uint src_line
#endif
  )
{
  int result;

#ifdef HAVE_PSI_RWLOCK_INTERFACE
  if (that->m_psi != NULL)
  {
    /* Instrumentation start */
    PSI_rwlock_locker *locker;
    PSI_rwlock_locker_state state;
    locker= PSI_RWLOCK_CALL(start_rwlock_wrwait)(&state, that->m_psi,
                                          PSI_RWLOCK_TRYWRITELOCK, src_file, src_line);

    /* Instrumented code */
    result= native_rw_trywrlock(&that->m_rwlock);

    /* Instrumentation end */
    if (locker != NULL)
      PSI_RWLOCK_CALL(end_rwlock_wrwait)(locker, result);

    return result;
  }
#endif

  /* Non instrumented code */
  result= native_rw_trywrlock(&that->m_rwlock);

  return result;
}

static inline int inline_myblockchain_rwlock_unlock(
  myblockchain_rwlock_t *that)
{
  int result;
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  if (that->m_psi != NULL)
    PSI_RWLOCK_CALL(unlock_rwlock)(that->m_psi);
#endif
  result= native_rw_unlock(&that->m_rwlock);
  return result;
}

#ifndef DISABLE_MYBLOCKCHAIN_PRLOCK_H
static inline int inline_myblockchain_prlock_unlock(
  myblockchain_prlock_t *that)
{
  int result;
#ifdef HAVE_PSI_RWLOCK_INTERFACE
  if (that->m_psi != NULL)
    PSI_RWLOCK_CALL(unlock_rwlock)(that->m_psi);
#endif
  result= rw_pr_unlock(&that->m_prlock);
  return result;
}
#endif

static inline void inline_myblockchain_cond_register(
#ifdef HAVE_PSI_COND_INTERFACE
  const char *category,
  PSI_cond_info *info,
  int count
#else
  const char *category __attribute__ ((unused)),
  void *info __attribute__ ((unused)),
  int count __attribute__ ((unused))
#endif
)
{
#ifdef HAVE_PSI_COND_INTERFACE
  PSI_COND_CALL(register_cond)(category, info, count);
#endif
}

static inline int inline_myblockchain_cond_init(
#ifdef HAVE_PSI_COND_INTERFACE
  PSI_cond_key key,
#endif
  myblockchain_cond_t *that)
{
#ifdef HAVE_PSI_COND_INTERFACE
  that->m_psi= PSI_COND_CALL(init_cond)(key, &that->m_cond);
#else
  that->m_psi= NULL;
#endif
  return native_cond_init(&that->m_cond);
}

static inline int inline_myblockchain_cond_destroy(
  myblockchain_cond_t *that)
{
#ifdef HAVE_PSI_COND_INTERFACE
  if (that->m_psi != NULL)
  {
    PSI_COND_CALL(destroy_cond)(that->m_psi);
    that->m_psi= NULL;
  }
#endif
  return native_cond_destroy(&that->m_cond);
}

static inline int inline_myblockchain_cond_wait(
  myblockchain_cond_t *that,
  myblockchain_mutex_t *mutex
#if defined(SAFE_MUTEX) || defined(HAVE_PSI_COND_INTERFACE)
  , const char *src_file, uint src_line
#endif
  )
{
  int result;

#ifdef HAVE_PSI_COND_INTERFACE
  if (that->m_psi != NULL)
  {
    /* Instrumentation start */
    PSI_cond_locker *locker;
    PSI_cond_locker_state state;
    locker= PSI_COND_CALL(start_cond_wait)(&state, that->m_psi, mutex->m_psi,
                                      PSI_COND_WAIT, src_file, src_line);

    /* Instrumented code */
    result= my_cond_wait(&that->m_cond, &mutex->m_mutex
#ifdef SAFE_MUTEX
                         , src_file, src_line
#endif
                         );

    /* Instrumentation end */
    if (locker != NULL)
      PSI_COND_CALL(end_cond_wait)(locker, result);

    return result;
  }
#endif

  /* Non instrumented code */
  result= my_cond_wait(&that->m_cond, &mutex->m_mutex
#ifdef SAFE_MUTEX
                       , src_file, src_line
#endif
                       );

  return result;
}

static inline int inline_myblockchain_cond_timedwait(
  myblockchain_cond_t *that,
  myblockchain_mutex_t *mutex,
  const struct timespec *abstime
#if defined(SAFE_MUTEX) || defined(HAVE_PSI_COND_INTERFACE)
  , const char *src_file, uint src_line
#endif
  )
{
  int result;

#ifdef HAVE_PSI_COND_INTERFACE
  if (that->m_psi != NULL)
  {
    /* Instrumentation start */
    PSI_cond_locker *locker;
    PSI_cond_locker_state state;
    locker= PSI_COND_CALL(start_cond_wait)(&state, that->m_psi, mutex->m_psi,
                                      PSI_COND_TIMEDWAIT, src_file, src_line);

    /* Instrumented code */
    result= my_cond_timedwait(&that->m_cond, &mutex->m_mutex, abstime
#ifdef SAFE_MUTEX
                              , src_file, src_line
#endif
                              );

    /* Instrumentation end */
    if (locker != NULL)
      PSI_COND_CALL(end_cond_wait)(locker, result);

    return result;
  }
#endif

  /* Non instrumented code */
  result= my_cond_timedwait(&that->m_cond, &mutex->m_mutex, abstime
#ifdef SAFE_MUTEX
                            , src_file, src_line
#endif
                            );

  return result;
}

static inline int inline_myblockchain_cond_signal(
  myblockchain_cond_t *that)
{
  int result;
#ifdef HAVE_PSI_COND_INTERFACE
  if (that->m_psi != NULL)
    PSI_COND_CALL(signal_cond)(that->m_psi);
#endif
  result= native_cond_signal(&that->m_cond);
  return result;
}

static inline int inline_myblockchain_cond_broadcast(
  myblockchain_cond_t *that)
{
  int result;
#ifdef HAVE_PSI_COND_INTERFACE
  if (that->m_psi != NULL)
    PSI_COND_CALL(broadcast_cond)(that->m_psi);
#endif
  result= native_cond_broadcast(&that->m_cond);
  return result;
}

static inline void inline_myblockchain_thread_register(
#ifdef HAVE_PSI_THREAD_INTERFACE
  const char *category,
  PSI_thread_info *info,
  int count
#else
  const char *category __attribute__ ((unused)),
  void *info __attribute__ ((unused)),
  int count __attribute__ ((unused))
#endif
)
{
#ifdef HAVE_PSI_THREAD_INTERFACE
  PSI_THREAD_CALL(register_thread)(category, info, count);
#endif
}

#ifdef HAVE_PSI_THREAD_INTERFACE
static inline int inline_myblockchain_thread_create(
  PSI_thread_key key,
  my_thread_handle *thread, const my_thread_attr_t *attr,
  my_start_routine start_routine, void *arg)
{
  int result;
  result= PSI_THREAD_CALL(spawn_thread)(key, thread, attr, start_routine, arg);
  return result;
}

static inline void inline_myblockchain_thread_set_psi_id(my_thread_id id)
{
  struct PSI_thread *psi= PSI_THREAD_CALL(get_thread)();
  PSI_THREAD_CALL(set_thread_id)(psi, id);
}

#ifdef __cplusplus
class THD;
static inline void inline_myblockchain_thread_set_psi_THD(THD *thd)
{
  struct PSI_thread *psi= PSI_THREAD_CALL(get_thread)();
  PSI_THREAD_CALL(set_thread_THD)(psi, thd);
}
#endif /* __cplusplus */

#endif

#endif /* DISABLE_MYBLOCKCHAIN_THREAD_H */

/** @} (end of group Thread_instrumentation) */

#endif

