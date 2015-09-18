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
#include "config.h"
#define SELECT_QUERY "select name from test where num = %d"


int main(int argc, char **argv)
{
#ifdef HAVE_OPENSSL
  int	count, num;
  MYBLOCKCHAIN myblockchain,*sock;
  MYBLOCKCHAIN_RES *res;
  char	qbuf[160];

  if (argc != 3)
  {
    fprintf(stderr,"usage : ssl_test <dbname> <num>\n\n");
    exit(1);
  }

  myblockchain_init(&myblockchain);
#ifdef HAVE_OPENSSL
  myblockchain_ssl_set(&myblockchain,"../SSL/MyBlockchain-client-key.pem",
    "../SSL/MyBlockchain-client-cert.pem",
    "../SSL/MyBlockchain-ca-cert.pem", 0, 0);
#endif
  if (!(sock = myblockchain_real_connect(&myblockchain,"127.0.0.1",0,0,argv[1],MYBLOCKCHAIN_PORT,NULL,0)))
  {
    fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",myblockchain_error(&myblockchain));
    perror("");
    exit(1);
  }
  myblockchain.reconnect= 1;
  count = 0;
  num = atoi(argv[2]);
  while (count < num)
  {
    sprintf(qbuf,SELECT_QUERY,count);
    if(myblockchain_query(sock,qbuf))
    {
      fprintf(stderr,"Query failed (%s)\n",myblockchain_error(sock));
      exit(1);
    }
    if (!(res=myblockchain_store_result(sock)))
    {
      fprintf(stderr,"Couldn't get result from query failed (%s)\n",
	      myblockchain_error(sock));
      exit(1);
    }
#ifdef TEST
    printf("number of fields: %d\n",myblockchain_num_fields(res));
#endif
    myblockchain_free_result(res);
    count++;
  }
  myblockchain_close(sock);
#else /* HAVE_OPENSSL */
  printf("ssl_test: SSL not configured.\n");
#endif /* HAVE_OPENSSL */
  exit(0);
  return 0;					/* Keep some compilers happy */
}
