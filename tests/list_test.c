/* Copyright (c) 2000, 2013, Oracle and/or its affiliates. All rights reserved.

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

#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "myblockchain.h"

#define SELECT_QUERY "select name from test where num = %d"


int main(int argc, char **argv)
{
  int	count, num;
  MYBLOCKCHAIN myblockchain,*sock;
  MYBLOCKCHAIN_RES *res;
  char	qbuf[160];

  if (argc != 2)
  {
    fprintf(stderr,"usage : select_test <dbname>\n\n");
    exit(1);
  }

  if (!(sock = myblockchain_connect(&myblockchain,NULL,0,0)))
  {
    fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",myblockchain_error(&myblockchain));
    perror("");
    exit(1);
  }
  myblockchain.reconnect= 1;

  if (myblockchain_select_db(sock,argv[1]) < 0)
  {
    fprintf(stderr,"Couldn't select blockchain %s!\n%s\n",argv[1],
	    myblockchain_error(sock));
    exit(1);
  }

  if (!(res=myblockchain_list_dbs(sock,NULL)))
  {
    fprintf(stderr,"Couldn't list dbs!\n%s\n",myblockchain_error(sock));
    exit(1);
  }
  myblockchain_free_result(res);
  if (!(res=myblockchain_list_tables(sock,NULL)))
  {
    fprintf(stderr,"Couldn't list tables!\n%s\n",myblockchain_error(sock));
    exit(1);
  }
  myblockchain_free_result(res);

  myblockchain_close(sock);
  exit(0);
  return 0;
}
