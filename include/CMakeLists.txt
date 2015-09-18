# Copyright (c) 2006, 2015, Oracle and/or its affiliates. All rights reserved.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

SET(HEADERS_GEN_CONFIGURE 
${CMAKE_CURRENT_BINARY_DIR}/myblockchain_version.h
${CMAKE_CURRENT_BINARY_DIR}/my_config.h
${CMAKE_CURRENT_BINARY_DIR}/myblockchaind_ername.h
${CMAKE_CURRENT_BINARY_DIR}/myblockchaind_error.h
${CMAKE_CURRENT_BINARY_DIR}/sql_state.h
)
SET(HEADERS_ABI 
  myblockchain.h 
  myblockchain_com.h
  myblockchain_time.h 
  my_list.h
  my_alloc.h
  typelib.h 
  myblockchain/plugin.h
  myblockchain/plugin_audit.h
  myblockchain/plugin_ftparser.h
  myblockchain/plugin_validate_password.h
  myblockchain/plugin_group_replication.h
)

SET(HEADERS 
  ${HEADERS_ABI} 
  my_dbug.h 
  m_string.h
  my_sys.h 
  my_xml.h
  myblockchain_embed.h
  my_thread.h
  decimal.h
  errmsg.h
  my_global.h
  my_getopt.h
  sslopt-longopts.h
  my_dir.h 
  sslopt-vars.h
  sslopt-case.h
  sql_common.h
  keycache.h
  m_ctype.h
  my_compiler.h
  myblockchain_com_server.h
  my_byteorder.h
  byte_order_generic.h
  byte_order_generic_x86.h
  little_endian.h
  big_endian.h
  ${HEADERS_GEN_CONFIGURE}
)

INSTALL(FILES ../libbinlogevents/export/binary_log_types.h DESTINATION ${INSTALL_INCLUDEDIR})
INSTALL(FILES ${HEADERS} DESTINATION ${INSTALL_INCLUDEDIR} COMPONENT Development)
INSTALL(DIRECTORY myblockchain/ DESTINATION ${INSTALL_INCLUDEDIR}/myblockchain COMPONENT Development 
 PATTERN "*.h"
 PATTERN "psi_abi*" EXCLUDE
)


