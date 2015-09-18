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

IF(WIN32)
  SET(localstatedir "C:\\\\myblockchain\\\\data\\\\")
  SET(ini_file_extension "ini")
  SET(CNF_SOCKET_LINE "")
ELSE()
  SET(localstatedir "${MYBLOCKCHAIN_DATADIR}")
  SET(prefix "${CMAKE_INSTALL_PREFIX}")
  SET(libexecdir "${CMAKE_INSTALL_PREFIX}/${INSTALL_SBINDIR}")
  SET(bindir "${CMAKE_INSTALL_PREFIX}/${INSTALL_BINDIR}" )
  SET(sbindir "${libexecdir}")
  SET(datadir "${MYBLOCKCHAIN_DATADIR}")
  SET(CC ${CMAKE_C_COMPILER})
  SET(CXX ${CMAKE_CXX_COMPILER})
  SET(CFLAGS ${CMAKE_C_FLAGS})
  SET(CXXFLAGS ${CMAKE_CXX_FLAGS})
  SET(MYBLOCKCHAIND_USER "myblockchain")
  SET(ini_file_extension "cnf")
  SET(HOSTNAME "hostname")
  SET(CNF_SOCKET_LINE "# socket = .....")
ENDIF()

  SET(dictionary "${CMAKE_INSTALL_PREFIX}/${INSTALL_SHAREDIR}/dictionary.txt" )

# XXX: shouldn't we just have variables for all this stuff and centralise
# XXX: their configuration in install_layout.cmake?
IF(WIN32)
  SET(inst_location ${INSTALL_DOCREADMEDIR})
ELSE()
  SET(inst_location ${INSTALL_SUPPORTFILESDIR})
ENDIF()

SET(inifile my-default)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/${inifile}.cnf.sh 
               ${CMAKE_CURRENT_BINARY_DIR}/${inifile}.${ini_file_extension} @ONLY)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${inifile}.${ini_file_extension}
        DESTINATION ${inst_location} COMPONENT IniFiles)

IF(UNIX)
  SET(prefix ${CMAKE_INSTALL_PREFIX})
  FOREACH(script  myblockchaind_multi.server myblockchain-log-rotate)
    CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/${script}.sh 
                   ${CMAKE_CURRENT_BINARY_DIR}/${script} @ONLY )

    IF(script MATCHES ".ini")
      SET(comp IniFiles)
    ELSE()
      SET(comp Server_Scripts)
    ENDIF()
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${script} 
       DESTINATION ${inst_location} COMPONENT ${comp}
       PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
  ENDFOREACH()
  IF(INSTALL_SUPPORTFILESDIR)
    INSTALL(FILES magic DESTINATION ${inst_location} COMPONENT SupportFiles)
  ENDIF()

  INSTALL(FILES myblockchain.m4 DESTINATION ${INSTALL_SHAREDIR}/aclocal COMPONENT Development)
  CONFIGURE_FILE(MyBlockchain-shared-compat.spec.sh ${CMAKE_CURRENT_BINARY_DIR}/MyBlockchain-shared-compat.spec @ONLY)
  
  SET(bindir ${prefix}/${INSTALL_BINDIR})
  SET(sbindir ${prefix}/${INSTALL_SBINDIR})
  SET(scriptdir ${prefix}/${INSTALL_SCRIPTDIR})
  SET(libexecdir ${prefix}/${INSTALL_SBINDIR})
  SET(pkgdatadir ${prefix}/${INSTALL_MYBLOCKCHAINSHAREDIR})
  CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/myblockchain.server.sh 
                 ${CMAKE_CURRENT_BINARY_DIR}/myblockchain.server @ONLY)
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/myblockchain.server 
    DESTINATION ${inst_location} COMPONENT SupportFiles
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ 
    GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
ENDIF()
