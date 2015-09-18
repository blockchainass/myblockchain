/* Copyright (c) 2003, 2014, Oracle and/or its affiliates. All rights reserved.
   
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

#ifndef CLIENT_SETTINGS_INCLUDED
#define CLIENT_SETTINGS_INCLUDED
#else
#error You have already included an client_settings.h and it should not be included twice
#endif /* CLIENT_SETTINGS_INCLUDED */

extern uint		myblockchain_port;
extern char *	myblockchain_unix_port;

/*
 Note: CLIENT_CAPABILITIES is also defined in sql/client_settings.h.
 When adding capabilities here, consider if they should be also added to
 the server's version.
*/
#define CLIENT_CAPABILITIES (CLIENT_LONG_PASSWORD | \
                             CLIENT_LONG_FLAG |     \
                             CLIENT_TRANSACTIONS |  \
                             CLIENT_PROTOCOL_41 | \
                             CLIENT_RESERVED2 | \
                             CLIENT_MULTI_RESULTS | \
                             CLIENT_PS_MULTI_RESULTS | \
                             CLIENT_PLUGIN_AUTH | \
                             CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA | \
                             CLIENT_CONNECT_ATTRS | \
                             CLIENT_SESSION_TRACK | \
                             CLIENT_DEPRECATE_EOF)

void read_user_name(char *name);
my_bool handle_local_infile(MYBLOCKCHAIN *myblockchain, const char *net_filename);

void myblockchain_read_default_options(struct st_myblockchain_options *options,
				const char *filename,const char *group);
void myblockchain_detach_stmt_list(LIST **stmt_list, const char *func_name);
MYBLOCKCHAIN * STDCALL
cli_myblockchain_real_connect(MYBLOCKCHAIN *myblockchain,const char *host, const char *user,
		       const char *passwd, const char *db,
		       uint port, const char *unix_socket,ulong client_flag);

void cli_myblockchain_close(MYBLOCKCHAIN *myblockchain);

MYBLOCKCHAIN_FIELD * cli_list_fields(MYBLOCKCHAIN *myblockchain);
my_bool cli_read_prepare_result(MYBLOCKCHAIN *myblockchain, MYBLOCKCHAIN_STMT *stmt);
MYBLOCKCHAIN_DATA * cli_read_rows(MYBLOCKCHAIN *myblockchain,MYBLOCKCHAIN_FIELD *myblockchain_fields,
				   uint fields);
int cli_stmt_execute(MYBLOCKCHAIN_STMT *stmt);
int cli_read_binary_rows(MYBLOCKCHAIN_STMT *stmt);
int cli_unbuffered_fetch(MYBLOCKCHAIN *myblockchain, char **row);
const char * cli_read_statistics(MYBLOCKCHAIN *myblockchain);
int cli_read_change_user_result(MYBLOCKCHAIN *myblockchain);

#ifdef EMBEDDED_LIBRARY
int init_embedded_server(int argc, char **argv, char **groups);
void end_embedded_server();
#endif /*EMBEDDED_LIBRARY*/

C_MODE_START
extern int myblockchain_init_character_set(MYBLOCKCHAIN *myblockchain);
C_MODE_END
