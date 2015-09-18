/* Copyright (c) 2012, 2015, Oracle and/or its affiliates. All rights reserved.

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

#ifndef MYBLOCKCHAIN_SERVICE_MYBLOCKCHAIN_ALLOC_INCLUDED
#define MYBLOCKCHAIN_SERVICE_MYBLOCKCHAIN_ALLOC_INCLUDED

#ifndef MYBLOCKCHAIN_ABI_CHECK
#include <stdlib.h>
#endif

/* PSI_memory_key */
#include "myblockchain/psi/psi_memory.h"

#ifdef __cplusplus
extern "C" {
#endif

/* myf */
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

#ifdef MYBLOCKCHAIN_DYNAMIC_PLUGIN

#define my_malloc myblockchain_malloc_service->myblockchain_malloc
#define my_realloc myblockchain_malloc_service->myblockchain_realloc
#define my_claim myblockchain_malloc_service->myblockchain_claim
#define my_free myblockchain_malloc_service->myblockchain_free
#define my_memdup myblockchain_malloc_service->my_memdup
#define my_strdup myblockchain_malloc_service->my_strdup
#define my_strndup myblockchain_malloc_service->my_strndup

#else

extern void * my_malloc(PSI_memory_key key, size_t size, myf_t flags);
extern void * my_realloc(PSI_memory_key key, void *ptr, size_t size, myf_t flags);
extern void my_claim(void *ptr);
extern void my_free(void *ptr);
extern void * my_memdup(PSI_memory_key key, const void *from, size_t length, myf_t flags);
extern char * my_strdup(PSI_memory_key key, const char *from, myf_t flags);
extern char * my_strndup(PSI_memory_key key, const char *from, size_t length, myf_t flags);

#endif

#ifdef __cplusplus
}
#endif

#endif

