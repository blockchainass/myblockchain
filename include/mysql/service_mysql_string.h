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

/* This service provides functions to parse myblockchain String */

#ifndef MYBLOCKCHAIN_SERVICE_MYBLOCKCHAIN_STRING_INCLUDED
#define MYBLOCKCHAIN_SERVICE_MYBLOCKCHAIN_STRING_INCLUDED

#ifndef MYBLOCKCHAIN_ABI_CHECK
#include <stdlib.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef void *myblockchain_string_iterator_handle;
typedef void *myblockchain_string_handle;

extern struct myblockchain_string_service_st {
  int (*myblockchain_string_convert_to_char_ptr_type)
       (myblockchain_string_handle, const char *, char *, unsigned int, int *);
  myblockchain_string_iterator_handle (*myblockchain_string_get_iterator_type)
                                (myblockchain_string_handle);
  int (*myblockchain_string_iterator_next_type)(myblockchain_string_iterator_handle);
  int (*myblockchain_string_iterator_isupper_type)(myblockchain_string_iterator_handle);
  int (*myblockchain_string_iterator_islower_type)(myblockchain_string_iterator_handle);
  int (*myblockchain_string_iterator_isdigit_type)(myblockchain_string_iterator_handle);
  myblockchain_string_handle (*myblockchain_string_to_lowercase_type)(myblockchain_string_handle);
  void (*myblockchain_string_free_type)(myblockchain_string_handle);
  void (*myblockchain_string_iterator_free_type)(myblockchain_string_iterator_handle);
} *myblockchain_string_service;

#ifdef MYBLOCKCHAIN_DYNAMIC_PLUGIN

#define myblockchain_string_convert_to_char_ptr(string_handle, charset_name, \
                                         buffer, buffer_size, error) \
        myblockchain_string_service->myblockchain_string_convert_to_char_ptr_type \
                              (string_handle, charset_name, buffer, \
                               buffer_size, error)

#define myblockchain_string_get_iterator(string_handle) \
        myblockchain_string_service->myblockchain_string_get_iterator_type(string_handle)

#define myblockchain_string_iterator_next(iterator_handle) \
        myblockchain_string_service->myblockchain_string_iterator_next_type(iterator_handle)

#define myblockchain_string_iterator_isupper(iterator_handle) \
        myblockchain_string_service->myblockchain_string_iterator_isupper_type \
                                     (iterator_handle)

#define myblockchain_string_iterator_islower(iterator_handle) \
        myblockchain_string_service->myblockchain_string_iterator_islower_type \
                                     (iterator_handle)

#define myblockchain_string_iterator_isdigit(iterator_handle) \
        myblockchain_string_service->myblockchain_string_iterator_isdigit_type \
                                     (iterator_handle)

#define myblockchain_string_to_lowercase(string_handle) \
        myblockchain_string_service->myblockchain_string_to_lowercase_type(string_handle)

#define myblockchain_string_free(myblockchain_string_handle) \
        myblockchain_string_service->myblockchain_string_free_type(myblockchain_string_handle)

#define myblockchain_string_iterator_free(myblockchain_string_iterator_handle) \
        myblockchain_string_service->myblockchain_string_iterator_free_type \
                                  (myblockchain_string_iterator_handle)
#else

/* This service function convert string into given character set */
int myblockchain_string_convert_to_char_ptr(myblockchain_string_handle string_handle,
                                     const char *charset_name, char *buffer,
                                     unsigned int buffer_size, int *error);

/* This service function returns the beginning of the iterator handle */
myblockchain_string_iterator_handle myblockchain_string_get_iterator(myblockchain_string_handle
                                                       string_handle);
/*  
  This service function gets the next iterator handle
  returns 0 if reached the end else return 1
*/
int myblockchain_string_iterator_next(myblockchain_string_iterator_handle iterator_handle);

/*  
  This service function return 1 if current iterator handle points to a
  uppercase character else return 0 for client character set.
*/
int myblockchain_string_iterator_isupper(myblockchain_string_iterator_handle iterator_handle);

/*  
  This service function return 1 if current iterator handle points to a
  lowercase character else return 0 for client character set.
*/
int myblockchain_string_iterator_islower(myblockchain_string_iterator_handle iterator_handle);

/*  
  This service function return 1 if current iterator handle points to a digit
  else return 0 for client character sets.
*/
int myblockchain_string_iterator_isdigit(myblockchain_string_iterator_handle iterator_handle);

/* convert string_handle into lowercase */
myblockchain_string_handle myblockchain_string_to_lowercase(myblockchain_string_handle
                                              string_handle);

/* It deallocates the string created on server side during plugin operations */
void myblockchain_string_free(myblockchain_string_handle);

/*  
  It deallocates the string iterator created on server side
  during plugin operations
*/
void myblockchain_string_iterator_free(myblockchain_string_iterator_handle);

#endif
#ifdef __cplusplus
}
#endif

#endif
