# Copyright (c) 2009, 2015, Oracle and/or its affiliates. All rights reserved.
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
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA 

#
# Global constants, only to be changed between major releases.
#

SET(SHARED_LIB_MAJOR_VERSION "20")
SET(SHARED_LIB_MINOR_VERSION "0")
SET(PROTOCOL_VERSION "10")
SET(DOT_FRM_VERSION "6")

# Generate "something" to trigger cmake rerun when VERSION changes
CONFIGURE_FILE(
  ${CMAKE_SOURCE_DIR}/VERSION
  ${CMAKE_BINARY_DIR}/VERSION.dep
)

# Read value for a variable from VERSION.

MACRO(MYBLOCKCHAIN_GET_CONFIG_VALUE keyword var)
 IF(NOT ${var})
   FILE (STRINGS ${CMAKE_SOURCE_DIR}/VERSION str REGEX "^[ ]*${keyword}=")
   IF(str)
     STRING(REPLACE "${keyword}=" "" str ${str})
     STRING(REGEX REPLACE  "[ ].*" ""  str "${str}")
     SET(${var} ${str})
   ENDIF()
 ENDIF()
ENDMACRO()


# Read myblockchain version for configure script

MACRO(GET_MYBLOCKCHAIN_VERSION)
  MYBLOCKCHAIN_GET_CONFIG_VALUE("MYBLOCKCHAIN_VERSION_MAJOR" MAJOR_VERSION)
  MYBLOCKCHAIN_GET_CONFIG_VALUE("MYBLOCKCHAIN_VERSION_MINOR" MINOR_VERSION)
  MYBLOCKCHAIN_GET_CONFIG_VALUE("MYBLOCKCHAIN_VERSION_PATCH" PATCH_VERSION)
  MYBLOCKCHAIN_GET_CONFIG_VALUE("MYBLOCKCHAIN_VERSION_EXTRA" EXTRA_VERSION)

  IF(NOT DEFINED MAJOR_VERSION OR
     NOT DEFINED MINOR_VERSION OR
     NOT DEFINED PATCH_VERSION)
    MESSAGE(FATAL_ERROR "VERSION file cannot be parsed.")
  ENDIF()

  SET(VERSION
    "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}${EXTRA_VERSION}")
  MESSAGE(STATUS "MyBlockchain ${VERSION}")
  SET(MYBLOCKCHAIN_BASE_VERSION
    "${MAJOR_VERSION}.${MINOR_VERSION}" CACHE INTERNAL "MyBlockchain Base version")
  SET(MYBLOCKCHAIN_NO_DASH_VERSION
    "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")

  # Use NDBVERSION irregardless of whether this is Cluster or not, if not
  # then the regex will be ignored anyway.
  STRING(REGEX REPLACE "^.*-ndb-" "" NDBVERSION "${VERSION}")
  STRING(REPLACE "-" "_" MYBLOCKCHAIN_RPM_VERSION "${NDBVERSION}")
  MATH(EXPR MYBLOCKCHAIN_VERSION_ID
    "10000*${MAJOR_VERSION} + 100*${MINOR_VERSION} + ${PATCH_VERSION}")
  MARK_AS_ADVANCED(VERSION MYBLOCKCHAIN_VERSION_ID MYBLOCKCHAIN_BASE_VERSION)
  SET(CPACK_PACKAGE_VERSION_MAJOR ${MAJOR_VERSION})
  SET(CPACK_PACKAGE_VERSION_MINOR ${MINOR_VERSION})
  SET(CPACK_PACKAGE_VERSION_PATCH ${PATCH_VERSION})
ENDMACRO()

# Get myblockchain version and other interesting variables
GET_MYBLOCKCHAIN_VERSION()

SET(SHARED_LIB_PATCH_VERSION ${PATCH_VERSION})

SET(MYBLOCKCHAIN_TCP_PORT_DEFAULT "3306")

IF(NOT MYBLOCKCHAIN_TCP_PORT)
  SET(MYBLOCKCHAIN_TCP_PORT ${MYBLOCKCHAIN_TCP_PORT_DEFAULT})
  SET(MYBLOCKCHAIN_TCP_PORT_DEFAULT "0")
ELSEIF(MYBLOCKCHAIN_TCP_PORT EQUAL MYBLOCKCHAIN_TCP_PORT_DEFAULT)
  SET(MYBLOCKCHAIN_TCP_PORT_DEFAULT "0")
ENDIF()


IF(NOT MYBLOCKCHAIN_UNIX_ADDR)
  SET(MYBLOCKCHAIN_UNIX_ADDR "/tmp/myblockchain.sock")
ENDIF()
IF(NOT COMPILATION_COMMENT)
  SET(COMPILATION_COMMENT "Source distribution")
ENDIF()

# Get the sys schema version from the myblockchain_sys_schema.sql file
# however if compiling without performance schema, always use version 1.0.0
MACRO(GET_SYS_SCHEMA_VERSION)
  IF(NOT WITH_PERFSCHEMA_STORAGE_ENGINE)
    SET(SYS_SCHEMA_VERSION "1.0.0")
  ELSE()
    FILE (STRINGS ${CMAKE_SOURCE_DIR}/scripts/myblockchain_sys_schema.sql str REGEX "SELECT \\'([0-9]+\\.[0-9]+\\.[0-9]+)\\' AS")
    IF(str)
      STRING(REGEX MATCH "([0-9]+\\.[0-9]+\\.[0-9]+)" SYS_SCHEMA_VERSION "${str}")
    ENDIF()
  ENDIF()
ENDMACRO()

GET_SYS_SCHEMA_VERSION()

INCLUDE(package_name)
IF(NOT CPACK_PACKAGE_FILE_NAME)
  GET_PACKAGE_FILE_NAME(CPACK_PACKAGE_FILE_NAME)
ENDIF()

IF(NOT CPACK_SOURCE_PACKAGE_FILE_NAME)
  SET(CPACK_SOURCE_PACKAGE_FILE_NAME "myblockchain-${VERSION}")
  IF("${VERSION}" MATCHES "-ndb-")
    STRING(REGEX REPLACE "^.*-ndb-" "" NDBVERSION "${VERSION}")
    SET(CPACK_SOURCE_PACKAGE_FILE_NAME "myblockchain-cluster-gpl-${NDBVERSION}")
  ENDIF()
ENDIF()
SET(CPACK_PACKAGE_CONTACT "MyBlockchain Release Engineering <myblockchain-build@oss.oracle.com>")
SET(CPACK_PACKAGE_VENDOR "Oracle Corporation")
SET(CPACK_SOURCE_GENERATOR "TGZ")
INCLUDE(cpack_source_ignore_files)

# Defintions for windows version resources
SET(PRODUCTNAME "MyBlockchain Server")
SET(COMPANYNAME ${CPACK_PACKAGE_VENDOR})

# Windows 'date' command has unpredictable output, so cannot rely on it to
# set MYBLOCKCHAIN_COPYRIGHT_YEAR - if someone finds a portable way to do so then
# it might be useful
#IF (WIN32)
#  EXECUTE_PROCESS(COMMAND "date" "/T" OUTPUT_VARIABLE TMP_DATE)
#  STRING(REGEX REPLACE "(..)/(..)/..(..).*" "\\3\\2\\1" MYBLOCKCHAIN_COPYRIGHT_YEAR ${TMP_DATE})
IF(UNIX)
  EXECUTE_PROCESS(COMMAND "date" "+%Y" OUTPUT_VARIABLE MYBLOCKCHAIN_COPYRIGHT_YEAR OUTPUT_STRIP_TRAILING_WHITESPACE)
ENDIF()

# Add version information to the exe and dll files
# Refer to http://msdn.microsoft.com/en-us/library/aa381058(VS.85).aspx
# for more info.
IF(MSVC)
    GET_FILENAME_COMPONENT(MYBLOCKCHAIN_CMAKE_SCRIPT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
	
    SET(FILETYPE VFT_APP)
    CONFIGURE_FILE(${MYBLOCKCHAIN_CMAKE_SCRIPT_DIR}/versioninfo.rc.in 
    ${CMAKE_BINARY_DIR}/versioninfo_exe.rc)

    SET(FILETYPE VFT_DLL)
    CONFIGURE_FILE(${MYBLOCKCHAIN_CMAKE_SCRIPT_DIR}/versioninfo.rc.in  
      ${CMAKE_BINARY_DIR}/versioninfo_dll.rc)
	  
  FUNCTION(ADD_VERSION_INFO target target_type sources_var)
    IF("${target_type}" MATCHES "SHARED" OR "${target_type}" MATCHES "MODULE")
      SET(rcfile ${CMAKE_BINARY_DIR}/versioninfo_dll.rc)
    ELSEIF("${target_type}" MATCHES "EXE")
      SET(rcfile ${CMAKE_BINARY_DIR}/versioninfo_exe.rc)
    ENDIF()
    SET(${sources_var} ${${sources_var}} ${rcfile} PARENT_SCOPE)
  ENDFUNCTION()
ELSE()
  FUNCTION(ADD_VERSION_INFO)
  ENDFUNCTION()
ENDIF()
