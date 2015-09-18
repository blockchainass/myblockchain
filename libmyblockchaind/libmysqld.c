/* Copyright (c) 2001, 2015, Oracle and/or its affiliates. All rights reserved.

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

#include <my_global.h>
#include <myblockchain.h>
#include <myblockchaind_error.h>
#include <my_thread.h>
#include <my_sys.h>
#include <mysys_err.h>
#include <m_string.h>
#include <m_ctype.h>
#include "errmsg.h"
#include <violite.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include <sql_common.h>
#include "embedded_priv.h"
#include "client_settings.h"
#ifdef	 HAVE_PWD_H
#include <pwd.h>
#endif
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif
#ifdef HAVE_SYS_UN_H
#  include <sys/un.h>
#endif
#ifndef INADDR_NONE
#define INADDR_NONE	-1
#endif

extern ulong net_buffer_length;
extern ulong max_allowed_packet;

#if defined(_WIN32)
#define ERRNO WSAGetLastError()
#define perror(A)
#else
#include <errno.h>
#define ERRNO errno
#define SOCKET_ERROR -1
#define closesocket(A) close(A)
#endif

#ifdef HAVE_GETPWUID
struct passwd *getpwuid(uid_t);
char* getlogin(void);
#endif

#ifdef _WIN32
static my_bool is_NT(void)
{
  char *os=getenv("OS");
  return (os && !strcmp(os, "Windows_NT")) ? 1 : 0;
}
#endif


int myblockchain_init_character_set(MYBLOCKCHAIN *myblockchain);

MYBLOCKCHAIN * STDCALL
myblockchain_real_connect(MYBLOCKCHAIN *myblockchain,const char *host, const char *user,
		   const char *passwd, const char *db,
		   uint port, const char *unix_socket,ulong client_flag)
{
  char name_buff[USERNAME_LENGTH + 1];

  DBUG_ENTER("myblockchain_real_connect");
  DBUG_PRINT("enter",("host: %s  db: %s  user: %s (libmyblockchaind)",
		      host ? host : "(Null)",
		      db ? db : "(Null)",
		      user ? user : "(Null)"));

  /* Test whether we're already connected */
  if (myblockchain->server_version)
  {
    set_myblockchain_error(myblockchain, CR_ALREADY_CONNECTED, unknown_sqlstate);
    DBUG_RETURN(0);
  }

  if (!host || !host[0])
    host= myblockchain->options.host;

  if (myblockchain->options.methods_to_use == MYBLOCKCHAIN_OPT_USE_REMOTE_CONNECTION ||
      (myblockchain->options.methods_to_use == MYBLOCKCHAIN_OPT_GUESS_CONNECTION &&
       host && *host && strcmp(host,LOCAL_HOST)))
    DBUG_RETURN(cli_myblockchain_real_connect(myblockchain, host, user, 
				       passwd, db, port, 
				       unix_socket, client_flag));

  myblockchain->methods= &embedded_methods;

  /* use default options */
  if (myblockchain->options.my_cnf_file || myblockchain->options.my_cnf_group)
  {
    myblockchain_read_default_options(&myblockchain->options,
			       (myblockchain->options.my_cnf_file ?
				myblockchain->options.my_cnf_file : "my"),
			       myblockchain->options.my_cnf_group);
    my_free(myblockchain->options.my_cnf_file);
    my_free(myblockchain->options.my_cnf_group);
    myblockchain->options.my_cnf_file=myblockchain->options.my_cnf_group=0;
  }

  if (!db || !db[0])
    db=myblockchain->options.db;

  if (!user || !user[0])
    user=myblockchain->options.user;

#ifndef NO_EMBEDDED_ACCESS_CHECKS
  if (!passwd)
  {
    passwd=myblockchain->options.password;
    if (!passwd)
      passwd=getenv("MYBLOCKCHAIN_PWD");		/* get it from environment */
  }
  myblockchain->passwd= passwd ? my_strdup(PSI_NOT_INSTRUMENTED,
                                    passwd,MYF(0)) : NULL;
#endif /*!NO_EMBEDDED_ACCESS_CHECKS*/
  if (!user || !user[0])
  {
    read_user_name(name_buff);
    if (name_buff[0])
      user= name_buff;
  }

  if (!user)
    user= "";
   /* 
      We need to alloc some space for myblockchain->info but don't want to
      put extra 'my_free's in myblockchain_close.
      So we alloc it with the 'user' string to be freed at once
   */
  myblockchain->user= my_strdup(PSI_NOT_INSTRUMENTED,
                         user, MYF(0));

  port=0;
  unix_socket=0;

  client_flag|=myblockchain->options.client_flag;
  /* Send client information for access check */
  client_flag|=CLIENT_CAPABILITIES;
  if (client_flag & CLIENT_MULTI_STATEMENTS)
    client_flag|= CLIENT_MULTI_RESULTS;
  /*
    no compression in embedded as we don't send any data,
    and no pluggable auth, as we cannot do a client-server dialog
  */
  client_flag&= ~(CLIENT_COMPRESS | CLIENT_PLUGIN_AUTH);
  if (db)
    client_flag|=CLIENT_CONNECT_WITH_DB;

  myblockchain->info_buffer= my_malloc(PSI_NOT_INSTRUMENTED,
                                MYBLOCKCHAIN_ERRMSG_SIZE, MYF(0));
  myblockchain->thd= create_embedded_thd(client_flag);

  init_embedded_myblockchain(myblockchain, client_flag);

  if (myblockchain_init_character_set(myblockchain))
    goto error;

  if (check_embedded_connection(myblockchain, db))
    goto error;

  myblockchain->server_status= SERVER_STATUS_AUTOCOMMIT;

  if (myblockchain->options.init_commands)
  {
    DYNAMIC_ARRAY *init_commands= myblockchain->options.init_commands;
    char **ptr= (char**)init_commands->buffer;
    char **end= ptr + init_commands->elements;

    for (; ptr<end; ptr++)
    {
      MYBLOCKCHAIN_RES *res;
      if (myblockchain_query(myblockchain,*ptr))
	goto error;
      if (myblockchain->fields)
      {
	if (!(res= (*myblockchain->methods->use_result)(myblockchain)))
	  goto error;
	myblockchain_free_result(res);
      }
    }
  }

  DBUG_PRINT("exit",("Mysql handler: 0x%lx", (long) myblockchain));
  DBUG_RETURN(myblockchain);

error:
  DBUG_PRINT("error",("message: %u (%s)",
                      myblockchain->net.last_errno,
                      myblockchain->net.last_error));
  {
    /* Free alloced memory */
    my_bool free_me=myblockchain->free_me;
    free_old_query(myblockchain); 
    myblockchain->free_me=0;
    myblockchain_close(myblockchain);
    myblockchain->free_me=free_me;
  }
  DBUG_RETURN(0);
}

