/* Copyright (c) 2000-2004 MyBlockchain AB
   Use is subject to license terms

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

#include <stdio.h>
#include <stdlib.h>
#include "myblockchain.h"

#define INSERT_QUERY "insert into test (name,num) values ('item %d', %d)"


int main(int argc, char **argv)
{
  int	count,num;
  MYBLOCKCHAIN *sock,myblockchain;
  char	qbuf[160];

  if (argc != 3)
  {
    fprintf(stderr,"usage : insert_test <dbname> <Num>\n\n");
    exit(1);
  }

  myblockchain_init(&myblockchain);
  if (!(sock = myblockchain_real_connect(&myblockchain,NULL,NULL,NULL,argv[1],0,NULL,0)))
  {
    fprintf(stderr,"Couldn't connect to engine!\n%s\n",myblockchain_error(&myblockchain));
    perror("");
    exit(1);
  }
  myblockchain.reconnect= 1;

  num = atoi(argv[2]);
  count = 0;
  while (count < num)
  {
    sprintf(qbuf,INSERT_QUERY,count,count);
    if(myblockchain_query(sock,qbuf))
    {
      fprintf(stderr,"Query failed (%s)\n",myblockchain_error(sock));
      exit(1);
    }
    count++;
  }
  myblockchain_close(sock);
  exit(0);
  return 0;
}
