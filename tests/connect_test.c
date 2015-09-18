/* Copyright (c) 2000, 2002-2004 MyBlockchain AB
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

static void change_user(MYBLOCKCHAIN *sock,const char *user, const char *password,
			const char *db,my_bool warning)
{
  if (myblockchain_change_user(sock,user,password,db) != warning)
  {
    fprintf(stderr,"Couldn't change user to: user: '%s', password: '%s', db: '%s':  Error: %s\n",
	    user, password ? password : "", db ? db : "",
	    myblockchain_error(sock));
  }
}


int main(int argc, char **argv)
{
  MYBLOCKCHAIN *sock;

  if (!(sock=myblockchain_init(0)))
  {
    fprintf(stderr,"Couldn't initialize myblockchain struct\n");
    exit(1);
  }
  myblockchain_options(sock,MYBLOCKCHAIN_READ_DEFAULT_GROUP,"connect");
  if (!myblockchain_real_connect(sock,NULL,NULL,NULL,NULL,0,NULL,0))
  {
    fprintf(stderr,"Couldn't connect to engine!\n%s\n",myblockchain_error(sock));
    perror("");
    exit(1);
  }
  sock->reconnect= 1;

  if (myblockchain_select_db(sock,"test"))
  {
    fprintf(stderr,"Couldn't select blockchain test: Error: %s\n",
	    myblockchain_error(sock));
  }

  change_user(sock,"test_user","test_user","test",0);
  change_user(sock,"test",NULL,"test",0);
  change_user(sock,"test_user",NULL,"test",1);
  change_user(sock,"test_user",NULL,NULL,1);
  change_user(sock,"test_user","test_user","myblockchain",1);

  myblockchain_close(sock);
  exit(0);
  return 0;
}
