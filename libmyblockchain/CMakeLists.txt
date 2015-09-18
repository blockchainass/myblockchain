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

INCLUDE_DIRECTORIES(
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/libmyblockchain
  ${CMAKE_SOURCE_DIR}/regex
  ${CMAKE_SOURCE_DIR}/sql
  ${CMAKE_SOURCE_DIR}/strings
  ${SSL_INCLUDE_DIRS}
  ${SSL_INTERNAL_INCLUDE_DIRS}
  ${ZLIB_INCLUDE_DIR})
ADD_DEFINITIONS(${SSL_DEFINES})

SET(CLIENT_API_FUNCTIONS
myblockchain_thread_end
myblockchain_thread_init
myblockchain_affected_rows
myblockchain_autocommit
myblockchain_stmt_bind_param
myblockchain_stmt_bind_result
myblockchain_change_user
myblockchain_character_set_name
myblockchain_close
myblockchain_commit
myblockchain_data_seek
myblockchain_debug
myblockchain_dump_debug_info
myblockchain_eof
myblockchain_errno
myblockchain_error
myblockchain_escape_string
myblockchain_hex_string
myblockchain_stmt_execute
myblockchain_stmt_fetch
myblockchain_stmt_fetch_column
myblockchain_fetch_field
myblockchain_fetch_field_direct
myblockchain_fetch_fields
myblockchain_fetch_lengths
myblockchain_fetch_row
myblockchain_field_count
myblockchain_field_seek
myblockchain_field_tell
myblockchain_free_result
myblockchain_get_client_info
myblockchain_get_host_info
myblockchain_get_proto_info
myblockchain_get_server_info
myblockchain_get_client_version
myblockchain_get_ssl_cipher
myblockchain_info
myblockchain_init
myblockchain_insert_id
myblockchain_kill
myblockchain_set_server_option
myblockchain_list_dbs
myblockchain_list_fields
myblockchain_list_processes
myblockchain_list_tables
myblockchain_more_results
myblockchain_next_result
myblockchain_num_fields
myblockchain_num_rows
myblockchain_options
myblockchain_stmt_param_count
myblockchain_stmt_param_metadata
myblockchain_ping
myblockchain_stmt_result_metadata
myblockchain_query
# We need to have document for this api
myblockchain_read_query_result
myblockchain_real_connect
myblockchain_real_escape_string
myblockchain_real_escape_string_quote
myblockchain_real_query
myblockchain_refresh
myblockchain_rollback
myblockchain_row_seek
myblockchain_row_tell
myblockchain_select_db
myblockchain_stmt_send_long_data
# We need to have document for this api
myblockchain_send_query
myblockchain_shutdown
myblockchain_ssl_set
myblockchain_stat
myblockchain_stmt_affected_rows
myblockchain_stmt_close
myblockchain_stmt_reset
myblockchain_stmt_data_seek
myblockchain_stmt_errno
myblockchain_stmt_error
myblockchain_stmt_free_result
myblockchain_stmt_num_rows
myblockchain_stmt_row_seek
myblockchain_stmt_row_tell
myblockchain_stmt_store_result
myblockchain_store_result
myblockchain_thread_id
myblockchain_thread_safe
myblockchain_use_result
myblockchain_warning_count
myblockchain_stmt_sqlstate
myblockchain_sqlstate
myblockchain_get_server_version
myblockchain_stmt_prepare
myblockchain_stmt_init
myblockchain_stmt_insert_id
myblockchain_stmt_attr_get
myblockchain_stmt_attr_set
myblockchain_stmt_field_count
myblockchain_set_local_infile_default
myblockchain_set_local_infile_handler
myblockchain_server_init
myblockchain_server_end
myblockchain_set_character_set
myblockchain_get_character_set_info
myblockchain_stmt_next_result
# Since its already documented we will keep it. FR will be created to redesign.
my_init
myblockchain_client_find_plugin
myblockchain_client_register_plugin
myblockchain_load_plugin
myblockchain_load_plugin_v
myblockchain_options4
myblockchain_plugin_options
myblockchain_reset_connection
myblockchain_get_option
myblockchain_session_track_get_first
myblockchain_session_track_get_next

CACHE INTERNAL "Functions exported by client API"

)

# Below list will have all the undocumented C API symbols but still exported.
# Once the decision is taken to have documentation we need to move them to
# CLIENT_API_FUNCTIONS list.
SET(CLIENT_API_FUNCTIONS_UNDOCUMENTED
get_tty_password
# my_load_defaults is a wrapper for load_defaults and it is not documented. We
# will have a FR to replace this for decent name/functionality and document it.
my_load_defaults
handle_options
myblockchain_get_parameters

CACHE INTERNAL "Undocumented Functions exported by client API"

)


SET(CLIENT_SOURCES
  get_password.c 
  libmyblockchain.c
  errmsg.c
  ../sql-common/client.c 
  ../sql-common/my_time.c 
  ../sql-common/client_plugin.c 
  ../sql-common/client_authentication.cc
  ../sql/net_serv.cc
  ../sql-common/pack.c 
  ../sql/auth/password.c
)

IF (WIN32 AND OPENSSL_APPLINK_C)
  SET_SOURCE_FILES_PROPERTIES(
    ../sql-common/client_authentication.cc
    PROPERTIES COMPILE_FLAGS "-DHAVE_OPENSSL_APPLINK_C"
  )
ENDIF()

#
# Include protocol tracing infrastructure and the test
# trace plugin if enabled by build options.
#
IF (WITH_CLIENT_PROTOCOL_TRACING)

  LIST(APPEND CLIENT_SOURCES myblockchain_trace.c)
  ADD_DEFINITIONS(-D CLIENT_PROTOCOL_TRACING)

  IF (WITH_TEST_TRACE_PLUGIN)
    MESSAGE(STATUS "Client library contains the test trace plugin")
    LIST(APPEND CLIENT_SOURCES test_trace_plugin.cc)
    ADD_DEFINITIONS(-D TEST_TRACE_PLUGIN)
  ENDIF (WITH_TEST_TRACE_PLUGIN)

ENDIF (WITH_CLIENT_PROTOCOL_TRACING)

ADD_CONVENIENCE_LIBRARY(clientlib ${CLIENT_SOURCES})
DTRACE_INSTRUMENT(clientlib)
ADD_DEPENDENCIES(clientlib GenError)

SET(LIBS clientlib dbug strings vio mysys mysys_ssl ${ZLIB_LIBRARY} ${SSL_LIBRARIES} ${LIBDL})

#
# On Windows platform client library includes the client-side 
# Windows Native Authentication plugin.
#
IF(WIN32)
  ADD_DEFINITIONS(-DAUTHENTICATION_WIN)
  ADD_SUBDIRECTORY(authentication_win)
  LIST(APPEND LIBS auth_win_client)
ENDIF()

# Merge several convenience libraries into one big myblockchainclient
MERGE_LIBRARIES(myblockchainclient STATIC ${LIBS} COMPONENT Development)

# Visual Studio users need debug  static library for debug projects
IF(MSVC)
 INSTALL_DEBUG_TARGET(myblockchainclient DESTINATION ${INSTALL_LIBDIR}/debug)
 INSTALL_DEBUG_TARGET(clientlib DESTINATION ${INSTALL_LIBDIR}/debug)
ENDIF()

MACRO(GET_TARGET_NAME target out_name)
  GET_TARGET_PROPERTY(location ${target} LOCATION)
  GET_FILENAME_COMPONENT(name ${location} NAME)
  SET(${out_name} ${name})
ENDMACRO()

IF(UNIX)
  MACRO(GET_VERSIONED_LIBNAME LIBNAME EXTENSION VERSION OUTNAME)
    SET(DOT_VERSION ".${VERSION}")
    IF(DOT_VERSION STREQUAL ".") 
      SET(DOT_VERSION "")
    ENDIF()
    IF(APPLE)
      SET(${OUTNAME} ${LIBNAME}${DOT_VERSION}${EXTENSION})
    ELSE()
      SET(${OUTNAME} ${LIBNAME}${EXTENSION}${DOT_VERSION})
    ENDIF() 
  ENDMACRO()
ENDIF()

IF(UNIX)
  GET_TARGET_NAME(myblockchainclient lib_name)
  INSTALL_SYMLINK(myblockchainclient
    ${lib_name} ${CMAKE_STATIC_LIBRARY_PREFIX}myblockchainclient_r.a
    ${INSTALL_LIBDIR} Development)
ENDIF()

# In our special "mixed" builds, also install libstlport which will
# be injected into the build tree by the Sun Studio part of the build.

IF(CMAKE_SYSTEM_NAME MATCHES "SunOS" AND DEFINED ENV{PB2WORKDIR})
  INSTALL(FILES ${CMAKE_BINARY_DIR}/libstlport.so
                ${CMAKE_BINARY_DIR}/libstlport.so.1
          DESTINATION ${INSTALL_LIBDIR} COMPONENT SharedLibraries OPTIONAL)
ENDIF()

IF(NOT DISABLE_SHARED)
  # Merge several convenience libraries into one big myblockchainclient
  # and link them together into shared library.
  MERGE_LIBRARIES(libmyblockchain SHARED ${LIBS}
    EXPORTS ${CLIENT_API_FUNCTIONS} ${CLIENT_API_FUNCTIONS_UNDOCUMENTED}
    COMPONENT SharedLibraries)
  IF(UNIX)
    # libtool compatability
    IF(CMAKE_SYSTEM_NAME MATCHES "FreeBSD" OR APPLE)
      SET(OS_SHARED_LIB_VERSION "${SHARED_LIB_MAJOR_VERSION}")
    ELSE()
      SET(OS_SHARED_LIB_VERSION
        "${SHARED_LIB_MAJOR_VERSION}.${SHARED_LIB_MINOR_VERSION}.${SHARED_LIB_PATCH_VERSION}")
    ENDIF()
    # Name of shared library is myblockchainclient on Unix
    SET_TARGET_PROPERTIES(libmyblockchain PROPERTIES 
      OUTPUT_NAME myblockchainclient 
      VERSION "${OS_SHARED_LIB_VERSION}" 
      SOVERSION "${SHARED_LIB_MAJOR_VERSION}")
    CONFIGURE_FILE(libmyblockchain.ver.in ${CMAKE_CURRENT_BINARY_DIR}/libmyblockchain.ver)

    IF(APPLE)
      SET_TARGET_PROPERTIES(libmyblockchain PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIBDIR}"
      )
    ENDIF()

    GET_TARGET_PROPERTY(libmyblockchain_link_flags libmyblockchain LINK_FLAGS)
    IF(NOT libmyblockchain_link_flag)
      SET(libmyblockchain_link_flags)
    ENDIF()
    SET(libmyblockchain_link_flags
      "${CMAKE_SHARED_LIBRARY_C_FLAGS} ${libmyblockchain_link_flags}")
    SET_TARGET_PROPERTIES(libmyblockchain
      PROPERTIES LINK_FLAGS "${libmyblockchain_link_flags}")

    IF(LINK_FLAG_NO_UNDEFINED)
      SET(libmyblockchain_link_flags
        "${libmyblockchain_link_flags} ${LINK_FLAG_NO_UNDEFINED}")
      SET(libmyblockchain_link_flags
        "${libmyblockchain_link_flags} -Wl,--version-script=${CMAKE_CURRENT_BINARY_DIR}/libmyblockchain.ver")
      SET_TARGET_PROPERTIES(libmyblockchain
        PROPERTIES LINK_FLAGS "${libmyblockchain_link_flags}")
    ENDIF() 
    # clean direct output needs to be set several targets have the same name
    #(myblockchainclient in this case)
    SET_TARGET_PROPERTIES(myblockchainclient PROPERTIES CLEAN_DIRECT_OUTPUT 1)
    SET_TARGET_PROPERTIES(libmyblockchain PROPERTIES CLEAN_DIRECT_OUTPUT 1)

    # Install links to libmyblockchainclient.so (client_r)
    GET_VERSIONED_LIBNAME(
      "${CMAKE_SHARED_LIBRARY_PREFIX}myblockchainclient_r"
      "${CMAKE_SHARED_LIBRARY_SUFFIX}"
      ""
      linkname)
    GET_TARGET_NAME(libmyblockchain lib_name)
    GET_FILENAME_COMPONENT(lib_name_we ${lib_name} NAME_WE)
    INSTALL_SYMLINK(libmyblockchain
      ${lib_name} ${linkname}
      ${INSTALL_LIBDIR} SharedLibraries)
    SET(OS_SHARED_LIB_SYMLINKS
      "${SHARED_LIB_MAJOR_VERSION}" "${OS_SHARED_LIB_VERSION}")
    LIST(REMOVE_DUPLICATES OS_SHARED_LIB_SYMLINKS)
    FOREACH(ver ${OS_SHARED_LIB_SYMLINKS})
      GET_VERSIONED_LIBNAME(
        "${CMAKE_SHARED_LIBRARY_PREFIX}myblockchainclient_r"
        "${CMAKE_SHARED_LIBRARY_SUFFIX}"
        "${ver}"
        linkname)
      GET_VERSIONED_LIBNAME(
        ${lib_name_we} "${CMAKE_SHARED_LIBRARY_SUFFIX}" "${ver}" lib_name_ver)
      INSTALL_SYMLINK(libmyblockchain
        ${lib_name_ver} ${linkname}
        ${INSTALL_LIBDIR} SharedLibraries)
    ENDFOREACH()
  ENDIF()
ENDIF()

#
#  Basic application for testing linking against dynamic client library.
#

#
# Generate a comma separated list of C API functions which is used
# to initialize api_calls[] array in api_test.c
#
SET(CLIENT_API_FUNCTION_LIST "")
FOREACH(api ${CLIENT_API_FUNCTIONS})
  SET(CLIENT_API_FUNCTION_LIST "${CLIENT_API_FUNCTION_LIST} ${api},")
ENDFOREACH()

#
# Generate api_test.c source, substituting @CLIENT_API_FUNCTION_LIST@
# with the list of API functions.
#
CONFIGURE_FILE(api_test.c.in ${CMAKE_CURRENT_BINARY_DIR}/api_test.c)

#
# Note: Compiling this test application will fail if not all symbols
# from @CLIENT_API_FUNCTIONS@ are declared by <myblockchain.h>. It will fail
# to run if not all of these symbols are exported by the library.
#
ADD_EXECUTABLE(libmyblockchain_api_test ${CMAKE_CURRENT_BINARY_DIR}/api_test.c)
SET_TARGET_PROPERTIES(libmyblockchain_api_test PROPERTIES LINKER_LANGUAGE CXX)
IF(LIBRT)
  TARGET_LINK_LIBRARIES(libmyblockchain_api_test ${LIBRT})
ENDIF()

IF(DISABLE_SHARED)
  TARGET_LINK_LIBRARIES(libmyblockchain_api_test ${LIBS})
ELSE()
  TARGET_LINK_LIBRARIES(libmyblockchain_api_test libmyblockchain)
ENDIF()
