/* Copyright (c) 2014, 2015 Oracle and/or its affiliates. All rights reserved.

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
  @file binary_log_types.h

  @brief This file contains the field type.


  @note This file can be imported both from C and C++ code, so the
  definitions have to be constructed to support this.
*/

#ifndef BINARY_LOG_TYPES_INCLUDED
#define BINARY_LOG_TYPES_INCLUDED

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Constants exported from this package.
 */

typedef enum enum_field_types {
  MYBLOCKCHAIN_TYPE_DECIMAL, MYBLOCKCHAIN_TYPE_TINY,
  MYBLOCKCHAIN_TYPE_SHORT,  MYBLOCKCHAIN_TYPE_LONG,
  MYBLOCKCHAIN_TYPE_FLOAT,  MYBLOCKCHAIN_TYPE_DOUBLE,
  MYBLOCKCHAIN_TYPE_NULL,   MYBLOCKCHAIN_TYPE_TIMESTAMP,
  MYBLOCKCHAIN_TYPE_LONGLONG,MYBLOCKCHAIN_TYPE_INT24,
  MYBLOCKCHAIN_TYPE_DATE,   MYBLOCKCHAIN_TYPE_TIME,
  MYBLOCKCHAIN_TYPE_DATETIME, MYBLOCKCHAIN_TYPE_YEAR,
  MYBLOCKCHAIN_TYPE_NEWDATE, MYBLOCKCHAIN_TYPE_VARCHAR,
  MYBLOCKCHAIN_TYPE_BIT,
  MYBLOCKCHAIN_TYPE_TIMESTAMP2,
  MYBLOCKCHAIN_TYPE_DATETIME2,
  MYBLOCKCHAIN_TYPE_TIME2,
  MYBLOCKCHAIN_TYPE_JSON=245,
  MYBLOCKCHAIN_TYPE_NEWDECIMAL=246,
  MYBLOCKCHAIN_TYPE_ENUM=247,
  MYBLOCKCHAIN_TYPE_SET=248,
  MYBLOCKCHAIN_TYPE_TINY_BLOB=249,
  MYBLOCKCHAIN_TYPE_MEDIUM_BLOB=250,
  MYBLOCKCHAIN_TYPE_LONG_BLOB=251,
  MYBLOCKCHAIN_TYPE_BLOB=252,
  MYBLOCKCHAIN_TYPE_VAR_STRING=253,
  MYBLOCKCHAIN_TYPE_STRING=254,
  MYBLOCKCHAIN_TYPE_GEOMETRY=255
} enum_field_types;

#define DATETIME_MAX_DECIMALS 6

#ifdef __cplusplus
}
#endif // __cplusplus

#endif /* BINARY_LOG_TYPES_INCLUDED */
