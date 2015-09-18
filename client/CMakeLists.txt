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
  ${CMAKE_SOURCE_DIR}/client
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/mysys_ssl
  ${ZLIB_INCLUDE_DIR}
  ${LZ4_INCLUDE_DIR}
  ${SSL_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/libmyblockchain
  ${CMAKE_SOURCE_DIR}/libbinlogevents/include
  ${CMAKE_SOURCE_DIR}/regex
  ${CMAKE_SOURCE_DIR}/strings
  ${EDITLINE_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)

INCLUDE(${MYBLOCKCHAIN_CMAKE_SCRIPT_DIR}/compile_flags.cmake)

# Prevent Boost from including external precompiled Boost libraries, use
# threading (not implemented for Solaris) and turn off unused functionality.
ADD_DEFINITIONS(
  -DBOOST_ALL_NO_LIB
  -DBOOST_SYSTEM_NO_DEPRECATED)

## Subdirectory with common client code.
ADD_SUBDIRECTORY(base)
## Subdirectory for myblockchainpump code.
ADD_SUBDIRECTORY(dump)

## We will need libeay32.dll and ssleay32.dll when running client executables.
COPY_OPENSSL_DLLS(copy_openssl_client)

INCLUDE(${MYBLOCKCHAIN_CMAKE_SCRIPT_DIR}/compile_flags.cmake)

SET(BOOST_SOURCES_DIR ${BOOST_PATCHES_DIR})

IF(WIN32)
  LIST(APPEND BOOST_THREAD_SOURCES
    ${BOOST_SOURCES_DIR}/libs/thread/src/win32/thread.cpp
    ${BOOST_SOURCES_DIR}/libs/thread/src/win32/tss_dll.cpp
    ${BOOST_SOURCES_DIR}/libs/thread/src/win32/tss_pe.cpp
  )
ELSE()
  LIST(APPEND BOOST_THREAD_SOURCES
    ${BOOST_SOURCES_DIR}/libs/thread/src/pthread/once.cpp
    ${BOOST_SOURCES_DIR}/libs/thread/src/pthread/thread.cpp
  )
ENDIF()

SET(BOOST_LIB_SOURCES
  ${BOOST_SOURCES_DIR}/libs/atomic/src/lockpool.cpp
  ${BOOST_SOURCES_DIR}/libs/chrono/src/chrono.cpp
  ${BOOST_SOURCES_DIR}/libs/chrono/src/process_cpu_clocks.cpp
  ${BOOST_SOURCES_DIR}/libs/chrono/src/thread_clock.cpp
  ${BOOST_SOURCES_DIR}/libs/system/src/error_code.cpp
  ${BOOST_SOURCES_DIR}/libs/thread/src/future.cpp
)

ADD_CONVENIENCE_LIBRARY(boost_lib
  ${BOOST_LIB_SOURCES}
  ${BOOST_THREAD_SOURCES}
)

SET_TARGET_PROPERTIES(boost_lib
  PROPERTIES COMPILE_FLAGS "-DBOOST_THREAD_BUILD_LIB")

# Do not build library unless it is needed by some other target.
SET_PROPERTY(TARGET boost_lib PROPERTY EXCLUDE_FROM_ALL TRUE)

MY_CHECK_CXX_COMPILER_FLAG("-Wno-logical-op" HAVE_NO_LOGICAL_OP)
IF(HAVE_NO_LOGICAL_OP)
  ADD_COMPILE_FLAGS(
    ${BOOST_SOURCES_DIR}/libs/thread/src/pthread/thread.cpp
    COMPILE_FLAGS "-Wno-logical-op"
  )
ENDIF()

# Boost source has unused local typedefs.
MY_CHECK_CXX_COMPILER_FLAG("-Wno-unused-local-typedefs" HAVE_NO_UNUSED_TYPEDEFS)
IF(HAVE_NO_UNUSED_TYPEDEFS)
  ADD_COMPILE_FLAGS(
    ${BOOST_SOURCES_DIR}/libs/thread/src/pthread/once.cpp
    ${BOOST_SOURCES_DIR}/libs/thread/src/pthread/thread.cpp
    COMPILE_FLAGS "-Wno-unused-local-typedefs"
  )
ENDIF()

ADD_COMPILE_FLAGS(
  ${BOOST_LIB_SOURCES}
  ${BOOST_THREAD_SOURCES}
  COMPILE_FLAGS -I${BOOST_PATCHES_DIR} -I${BOOST_INCLUDE_DIR}
)

# Need explicit pthread for gcc -fsanitize=address
IF(CMAKE_USE_PTHREADS_INIT AND CMAKE_C_FLAGS MATCHES "-fsanitize=")
  TARGET_LINK_LIBRARIES(boost_lib pthread)
ENDIF()

ADD_DEFINITIONS(${SSL_DEFINES})
MYBLOCKCHAIN_ADD_EXECUTABLE(myblockchain completion_hash.cc myblockchain.cc readline.cc ../sql-common/sql_string.cc)
TARGET_LINK_LIBRARIES(myblockchain myblockchainclient)
IF(UNIX)
  TARGET_LINK_LIBRARIES(myblockchain ${EDITLINE_LIBRARY})
ENDIF(UNIX)

IF(NOT WITHOUT_SERVER)
  MYBLOCKCHAIN_ADD_EXECUTABLE(myblockchain_upgrade
    upgrade/program.cc
  )
ADD_COMPILE_FLAGS(
  upgrade/program.cc COMPILE_FLAGS -I${BOOST_PATCHES_DIR} -I${BOOST_INCLUDE_DIR}
)
TARGET_LINK_LIBRARIES(myblockchain_upgrade myblockchainclient client_base myblockchaincheck_core)
ADD_DEPENDENCIES(myblockchain_upgrade GenFixPrivs GenSysSchema)
ENDIF()

MYBLOCKCHAIN_ADD_EXECUTABLE(myblockchaintest myblockchaintest.cc COMPONENT Test)
SET_SOURCE_FILES_PROPERTIES(myblockchaintest.cc PROPERTIES COMPILE_FLAGS "-DTHREADS")
TARGET_LINK_LIBRARIES(myblockchaintest myblockchainclient regex)

ADD_CONVENIENCE_LIBRARY(myblockchaincheck_core check/myblockchaincheck_core.cc)
TARGET_LINK_LIBRARIES(myblockchaincheck_core myblockchainclient)

MYBLOCKCHAIN_ADD_EXECUTABLE(myblockchaincheck check/myblockchaincheck.cc)
TARGET_LINK_LIBRARIES(myblockchaincheck myblockchaincheck_core)

MYBLOCKCHAIN_ADD_EXECUTABLE(myblockchaindump myblockchaindump.c ../sql-common/my_user.c)
TARGET_LINK_LIBRARIES(myblockchaindump myblockchainclient)

MYBLOCKCHAIN_ADD_EXECUTABLE(myblockchainimport myblockchainimport.c)
SET_SOURCE_FILES_PROPERTIES(myblockchainimport.c PROPERTIES COMPILE_FLAGS "-DTHREADS")
TARGET_LINK_LIBRARIES(myblockchainimport myblockchainclient)

MYBLOCKCHAIN_ADD_EXECUTABLE(myblockchainshow myblockchainshow.c)
TARGET_LINK_LIBRARIES(myblockchainshow myblockchainclient)

MYBLOCKCHAIN_ADD_EXECUTABLE(myblockchain_plugin myblockchain_plugin.c)
TARGET_LINK_LIBRARIES(myblockchain_plugin myblockchainclient)

MYBLOCKCHAIN_ADD_EXECUTABLE(myblockchainbinlog myblockchainbinlog.cc)
ADD_COMPILE_FLAGS(
  myblockchainbinlog.cc
  COMPILE_FLAGS "-I${CMAKE_SOURCE_DIR}/sql" "-DHAVE_REPLICATION"
)
TARGET_LINK_LIBRARIES(myblockchainbinlog myblockchainclient binlogevents_static)

MYBLOCKCHAIN_ADD_EXECUTABLE(myblockchainadmin myblockchainadmin.cc)
TARGET_LINK_LIBRARIES(myblockchainadmin myblockchainclient)

MYBLOCKCHAIN_ADD_EXECUTABLE(myblockchainslap myblockchainslap.cc)
SET_SOURCE_FILES_PROPERTIES(myblockchainslap.cc PROPERTIES COMPILE_FLAGS "-DTHREADS")
TARGET_LINK_LIBRARIES(myblockchainslap myblockchainclient)

MYBLOCKCHAIN_ADD_EXECUTABLE(myblockchain_config_editor myblockchain_config_editor.cc)
TARGET_LINK_LIBRARIES(myblockchain_config_editor myblockchainclient)

MYBLOCKCHAIN_ADD_EXECUTABLE(myblockchain_secure_installation myblockchain_secure_installation.cc)
TARGET_LINK_LIBRARIES(myblockchain_secure_installation myblockchainclient)

IF(UNIX AND NOT WITHOUT_SERVER)
  MYBLOCKCHAIN_ADD_EXECUTABLE(myblockchain_install_db
    myblockchain_install_db.cc auth_utils.cc path.cc logger.cc)
  TARGET_LINK_LIBRARIES(myblockchain_install_db myblockchainclient)
  ADD_COMPILE_FLAGS(
    auth_utils.cc myblockchain_install_db.cc
    COMPILE_FLAGS "-I${CMAKE_SOURCE_DIR}/sql/auth"
    )
  ADD_DEPENDENCIES(myblockchain_install_db GenBootstrapPriv GenSysSchema)
ENDIF()

MYBLOCKCHAIN_ADD_EXECUTABLE(myblockchain_ssl_rsa_setup myblockchain_ssl_rsa_setup.cc path.cc logger.cc)
TARGET_LINK_LIBRARIES(myblockchain_ssl_rsa_setup mysys mysys_ssl)
SET_TARGET_PROPERTIES(myblockchain_ssl_rsa_setup PROPERTIES LINKER_LANGUAGE CXX)

# "WIN32" also covers 64 bit. "echo" is used in some files below "myblockchain-test/".
IF(WIN32)
  MYBLOCKCHAIN_ADD_EXECUTABLE(echo echo.c)
ENDIF(WIN32)

SET_TARGET_PROPERTIES (
  myblockchain_plugin
  myblockchaincheck
  myblockchaindump
  myblockchainimport
  myblockchainshow
  myblockchainslap
PROPERTIES HAS_CXX TRUE)

ADD_DEFINITIONS(-DHAVE_DLOPEN)

