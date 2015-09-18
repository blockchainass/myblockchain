/* Copyright (c) 2004, 2011, Oracle and/or its affiliates. All rights reserved.

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA */

#ifndef _myblockchain_time_h_
#define _myblockchain_time_h_

/*
  Time declarations shared between the server and client API:
  you should not add anything to this header unless it's used
  (and hence should be visible) in myblockchain.h.
  If you're looking for a place to add new time-related declaration,
  it's most likely my_time.h. See also "C API Handling of Date
  and Time Values" chapter in documentation.
*/

enum enum_myblockchain_timestamp_type
{
  MYBLOCKCHAIN_TIMESTAMP_NONE= -2, MYBLOCKCHAIN_TIMESTAMP_ERROR= -1,
  MYBLOCKCHAIN_TIMESTAMP_DATE= 0, MYBLOCKCHAIN_TIMESTAMP_DATETIME= 1, MYBLOCKCHAIN_TIMESTAMP_TIME= 2
};


/*
  Structure which is used to represent datetime values inside MyBlockchain.

  We assume that values in this structure are normalized, i.e. year <= 9999,
  month <= 12, day <= 31, hour <= 23, hour <= 59, hour <= 59. Many functions
  in server such as my_system_gmt_sec() or make_time() family of functions
  rely on this (actually now usage of make_*() family relies on a bit weaker
  restriction). Also functions that produce MYBLOCKCHAIN_TIME as result ensure this.
  There is one exception to this rule though if this structure holds time
  value (time_type == MYBLOCKCHAIN_TIMESTAMP_TIME) days and hour member can hold
  bigger values.
*/
typedef struct st_myblockchain_time
{
  unsigned int  year, month, day, hour, minute, second;
  unsigned long second_part;  /**< microseconds */
  my_bool       neg;
  enum enum_myblockchain_timestamp_type time_type;
} MYBLOCKCHAIN_TIME;

#endif /* _myblockchain_time_h_ */
