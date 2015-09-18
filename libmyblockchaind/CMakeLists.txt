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

ADD_DEFINITIONS(-DMYBLOCKCHAIN_SERVER -DEMBEDDED_LIBRARY
 ${SSL_DEFINES})

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/include 
  ${CMAKE_SOURCE_DIR}/libmyblockchain
  ${CMAKE_SOURCE_DIR}/libbinlogevents/include
  ${CMAKE_SOURCE_DIR}/libmyblockchaind
  ${CMAKE_SOURCE_DIR}/sql
  ${CMAKE_SOURCE_DIR}/sql/auth
  ${CMAKE_SOURCE_DIR}/sql/conn_handler
  ${CMAKE_BINARY_DIR}/sql 
  ${CMAKE_SOURCE_DIR}/regex
  ${ZLIB_INCLUDE_DIR}
  ${SSL_INCLUDE_DIRS}
  ${SSL_INTERNAL_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/sql/backup
)

# We should generate these separately for libmyblockchaind to avoid
# compiling them for libmyblockchaind while they are generated for sql.
SET(GEN_YACC_SOURCES
  ${CMAKE_CURRENT_BINARY_DIR}/sql_yacc.h 
  ${CMAKE_CURRENT_BINARY_DIR}/sql_yacc.cc
  ${CMAKE_CURRENT_BINARY_DIR}/sql_hints.yy.h
  ${CMAKE_CURRENT_BINARY_DIR}/sql_hints.yy.cc
)

SET(GEN_SOURCES
  ${CMAKE_BINARY_DIR}/sql/sql_builtin.cc
  ${CMAKE_BINARY_DIR}/sql/lex_hash.h 
)

SET_SOURCE_FILES_PROPERTIES(${GEN_YACC_SOURCES}
                            ${GEN_SOURCES}
                            PROPERTIES GENERATED TRUE)

FOREACH(file ${SQL_EXPORTED_SOURCES})
  LIST(APPEND IMPORTED_SOURCES "../sql/${file}")
ENDFOREACH()

SET(SQL_EMBEDDED_SOURCES
  emb_qcache.cc
  lib_sql.cc 
  libmyblockchaind.c
  ${GEN_SOURCES}
  ${GEN_YACC_SOURCES}
  ../client/get_password.c
  ../libmyblockchain/errmsg.c
  ../libmyblockchain/libmyblockchain.c
  ../sql-common/client.c
  ../sql-common/client_plugin.c
  ../sql-common/my_time.c 
  ../sql-common/my_user.c
  ../sql-common/pack.c
  ../sql-common/sql_string.cc
  ../libbinlogevents/src/statement_events.cpp
  ../libbinlogevents/src/control_events.cpp
  ../libbinlogevents/src/load_data_events.cpp
  ../libbinlogevents/src/rows_event.cpp
  ../libbinlogevents/src/binlog_event.cpp
  ../libbinlogevents/src/binary_log_funcs.cpp
  ../libbinlogevents/src/uuid.cpp
  ../sql/binlog.cc 
  ../sql/event_parse_data.cc
  ../sql/log_event.cc
  ../sql/myblockchaind_thd_manager.cc
  ../sql/rpl_filter.cc
  ../sql/rpl_injector.cc
  ../sql/rpl_record.cc
  ../sql/rpl_reporting.cc
  ../sql/rpl_utility.cc
  ../sql/rpl_gtid_misc.cc
  ../sql/rpl_gtid_sid_map.cc
  ../sql/rpl_gtid_set.cc
  ../sql/rpl_gtid_specification.cc
  ../sql/rpl_gtid_state.cc
  ../sql/rpl_gtid_owned.cc
  ../sql/rpl_gtid_execution.cc
  ../sql/rpl_gtid_mutex_cond_array.cc
  ../sql/rpl_gtid_persist.cc
  ../sql/rpl_table_access.cc
  ../sql/rpl_context.cc
  ../sql/rpl_trx_boundary_parser.cc
  ${IMPORTED_SOURCES}
)

INCLUDE(${MYBLOCKCHAIN_CMAKE_SCRIPT_DIR}/compile_flags.cmake)
ADD_COMPILE_FLAGS(
  ../sql/handler.cc
  ../sql/item_geofunc.cc
  ../sql/item_geofunc_buffer.cc
  ../sql/item_geofunc_relchecks.cc
  ../sql/item_geofunc_relchecks_bgwrap.cc
  ../sql/item_geofunc_setops.cc
  ../sql/item_json_func.cc
  COMPILE_FLAGS -I${BOOST_PATCHES_DIR} -I${BOOST_INCLUDE_DIR}
)

# Boost source has unused local typedefs.
MY_CHECK_CXX_COMPILER_FLAG("-Wno-unused-local-typedefs" HAVE_NO_UNUSED_TYPEDEFS)
IF(HAVE_NO_UNUSED_TYPEDEFS)
  ADD_COMPILE_FLAGS(
    ../sql/handler.cc
    ../sql/item_geofunc.cc
    ../sql/item_geofunc_buffer.cc
    ../sql/item_geofunc_relchecks.cc
    ../sql/item_geofunc_relchecks_bgwrap.cc
    ../sql/item_geofunc_setops.cc
    ../sql/item_json_func.cc
    COMPILE_FLAGS "-Wno-unused-local-typedefs"
  )
ENDIF()

# Handle out-of-source build from source package with possibly broken 
# bison. Copy bison output to from source to build directory, if not already 
# there
INCLUDE(${CMAKE_SOURCE_DIR}/cmake/bison.cmake)
COPY_BISON_OUTPUT(
  ${CMAKE_SOURCE_DIR}/sql/sql_yacc.cc
  ${CMAKE_SOURCE_DIR}/sql/sql_yacc.h
  ${CMAKE_CURRENT_BINARY_DIR}/sql_yacc.cc
  ${CMAKE_CURRENT_BINARY_DIR}/sql_yacc.h
)

COPY_BISON_OUTPUT(
  ${CMAKE_SOURCE_DIR}/sql/sql_hints.yy.cc
  ${CMAKE_SOURCE_DIR}/sql/sql_hints.yy.h
  ${CMAKE_CURRENT_BINARY_DIR}/sql_hints.yy.cc
  ${CMAKE_CURRENT_BINARY_DIR}/sql_hints.yy.h
)

RUN_BISON(
  ${CMAKE_SOURCE_DIR}/sql/sql_yacc.yy 
  ${CMAKE_CURRENT_BINARY_DIR}/sql_yacc.cc
  ${CMAKE_CURRENT_BINARY_DIR}/sql_yacc.h
  MYBLOCKCHAIN
)

RUN_BISON(
  ${CMAKE_SOURCE_DIR}/sql/sql_hints.yy
  ${CMAKE_CURRENT_BINARY_DIR}/sql_hints.yy.cc
  ${CMAKE_CURRENT_BINARY_DIR}/sql_hints.yy.h
  HINT_PARSER_
)

ADD_CUSTOM_TARGET(GenYaccEmbeddedSource DEPENDS ${GEN_YACC_SOURCES})

ADD_CONVENIENCE_LIBRARY(sql_embedded ${SQL_EMBEDDED_SOURCES})
DTRACE_INSTRUMENT(sql_embedded)
ADD_DEPENDENCIES(sql_embedded GenError GenServerSource GenYaccEmbeddedSource)

# On Windows, static embedded server library is called myblockchainserver.lib
# On Unix, it is libmyblockchaind.a
IF(WIN32)
  SET(MYBLOCKCHAINSERVER_OUTPUT_NAME myblockchainserver)
ELSE()
  SET(MYBLOCKCHAINSERVER_OUTPUT_NAME myblockchaind)
ENDIF()


SET(LIBS 
  dbug strings regex mysys mysys_ssl vio
  ${ZLIB_LIBRARY} ${SSL_LIBRARIES}
  ${LIBCRYPT} ${LIBDL}
  ${MYBLOCKCHAIND_STATIC_EMBEDDED_PLUGIN_LIBS}
  sql_embedded
)

# Some storage engine were compiled for embedded specifically
# (with corresponding target ${engine}_embedded)
SET(EMBEDDED_LIBS)
FOREACH(LIB ${LIBS})
  GET_TARGET_PROPERTY(EMBEDDED_LOCATION ${LIB}_embedded LOCATION)
  IF(EMBEDDED_LOCATION)
    LIST(APPEND EMBEDDED_LIBS ${LIB}_embedded)
  ELSE()
    LIST(APPEND EMBEDDED_LIBS ${LIB})
  ENDIF()
ENDFOREACH()

MERGE_LIBRARIES(myblockchainserver STATIC ${EMBEDDED_LIBS} 
  OUTPUT_NAME ${MYBLOCKCHAINSERVER_OUTPUT_NAME} COMPONENT Embedded)

# Visual Studio users need debug  static library 
IF(MSVC)
 INSTALL_DEBUG_TARGET(myblockchainserver DESTINATION ${INSTALL_LIBDIR}/debug)
ENDIF()

IF(UNIX)
 INSTALL_DEBUG_TARGET(myblockchainserver DESTINATION ${INSTALL_LIBDIR} RENAME
  ${CMAKE_STATIC_LIBRARY_PREFIX}myblockchaind-debug)
ENDIF()

IF(MSVC AND NOT DISABLE_SHARED)
  MERGE_LIBRARIES(libmyblockchaind SHARED ${LIBS} EXPORTS ${CLIENT_API_FUNCTIONS}
  COMPONENT Embedded)
ENDIF()

OPTION(WITH_EMBEDDED_SHARED_LIBRARY
  "Generate shared version of embedded library (in addition to the static one)"
  OFF)
IF(WITH_EMBEDDED_SHARED_LIBRARY)
  MERGE_LIBRARIES(libmyblockchaind SHARED
    myblockchainserver
    EXPORTS ${CLIENT_API_FUNCTIONS}
    OUTPUT_NAME "myblockchaind"
    COMPONENT Embedded)
  SET_TARGET_PROPERTIES(libmyblockchaind PROPERTIES
    VERSION "${SHARED_LIB_MAJOR_VERSION}.${SHARED_LIB_MINOR_VERSION}.0"
    SOVERSION "${SHARED_LIB_MAJOR_VERSION}"
    )

  GET_TARGET_PROPERTY(libmyblockchaind_link_flags libmyblockchaind LINK_FLAGS)
  IF(NOT libmyblockchaind_link_flag)
    SET(libmyblockchaind_link_flags)
  ENDIF()
  SET(libmyblockchaind_link_flags
    "${CMAKE_SHARED_LIBRARY_C_FLAGS} ${libmyblockchaind_link_flags}")
  SET_TARGET_PROPERTIES(libmyblockchaind
    PROPERTIES LINK_FLAGS "${libmyblockchaind_link_flags}")

  IF(LINK_FLAG_NO_UNDEFINED)
    SET(libmyblockchaind_link_flags
      "${libmyblockchaind_link_flags} ${LINK_FLAG_NO_UNDEFINED}")
    SET_TARGET_PROPERTIES(libmyblockchaind
      PROPERTIES LINK_FLAGS "${libmyblockchaind_link_flags}")
  ENDIF()

  SET_TARGET_PROPERTIES(libmyblockchaind PROPERTIES CLEAN_DIRECT_OUTPUT 1)
  SET_TARGET_PROPERTIES(myblockchainserver PROPERTIES CLEAN_DIRECT_OUTPUT 1)
ENDIF()
