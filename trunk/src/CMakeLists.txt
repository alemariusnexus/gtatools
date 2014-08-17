# Copyright 2010-2014 David "Alemarius Nexus" Lerch
# 
# This file is part of gtatools.
#
# gtatools is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# gtatools is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with gtatools.  If not, see <http://www.gnu.org/licenses/>.
#
# Additional permissions are granted, which are listed in the file
# GPLADDITIONS.



CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(gtatools C CXX)

IF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/nxcommon/src/CMakeLists.txt")
    SET(NXCOMMON_BUILDING ON)
ENDIF(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/nxcommon/src/CMakeLists.txt")

#IF(NXCOMMON_BUILDING)
#    SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/nxcommon/src/cmakemodules)
#ELSE()
#    SET(NXCOMMON_CMAKE_MODULE_PATH "" CACHE PATH "Path to the nxcommon CMake module directory.")
#    
#    IF(NOT EXISTS "${NXCOMMON_CMAKE_MODULE_PATH}/NxUtilities.cmake")
#        MESSAGE(FATAL_ERROR "NXCOMMON_CMAKE_MODULE_PATH is invalid. You need to set it in order to build gtatools.")
#    ENDIF()
#    
#    SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${NXCOMMON_CMAKE_MODULE_PATH}")
#ENDIF()
#
#SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmakemodules")
#
#INCLUDE(NxUtilities)

SET(GTATOOLS_VERSION_MAJOR "0") # Major version, must be numeric
SET(GTATOOLS_VERSION_MINOR "2") # Minor version, must be numeric
SET(GTATOOLS_VERSION_SUFFIX "-dev") # A suffix for the version string, could e.g. be 'b' for beta versions, or empty

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "gtatools - A collection of tools and libraries for loading formats of the GTA game series.")
SET(CPACK_PACKAGE_VENDOR "David 'Alemarius Nexus' Lerch")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
#SET(CPACK_PACKAGE_VERSION_MAJOR ${GTATOOLS_VERSION_MAJOR})
#SET(CPACK_PACKAGE_VERSION_MINOR ${GTATOOLS_VERSION_MINOR})
#SET(CPACK_PACKAGE_VERSION_PATCH ${GTATOOLS_VERSION_PATCH}${GTATOOLS_VERSION_SUFFIX})
SET(CPACK_PACKAGE_VERSION "${GTATOOLS_VERSION_MAJOR}.${GTATOOLS_VERSION_MINOR}${GTATOOLS_VERSION_SUFFIX}")
SET(CPACK_SOURCE_IGNORE_FILES "\\\\.svn;.*~")


#ADD_DEFINITIONS(-D__STDC_LIMIT_MACROS -DUCHAR_TYPE=char16_t)

# --std=c++11 is usually enough, but GTest sometimes complains on MinGW when we don't enable GNU extensions.
#ADD_COMPILE_OPTIONS(--std=gnu++11)


FIND_PACKAGE(Subversion)

SET(_CMAKE_GTATOOLS_HAVE_SVNREV OFF)
IF(EXISTS "${CMAKE_SOURCE_DIR}/.svn" OR EXISTS "${CMAKE_SOURCE_DIR}/../.svn")
    IF(Subversion_FOUND)
        Subversion_WC_INFO(${CMAKE_SOURCE_DIR} Project)
        SET(GTATOOLS_VERSION_SVNREV "${Project_WC_REVISION}")
        SET(_CMAKE_GTATOOLS_HAVE_SVNREV ON)
    ELSE(Subversion_FOUND)
        MESSAGE(WARNING "Subversion client not found. The current subversion revision number will not be available to gtatools!")
    ENDIF(Subversion_FOUND)
ENDIF(EXISTS "${CMAKE_SOURCE_DIR}/.svn" OR EXISTS "${CMAKE_SOURCE_DIR}/../.svn")

IF(GTATOOLS_VERSION_SVNREV)
    MESSAGE(STATUS "Subversion revision of gtatools is r${GTATOOLS_VERSION_SVNREV}.")
    
    SET(GTATOOLS_VERSION "${GTATOOLS_VERSION_MAJOR}.${GTATOOLS_VERSION_MINOR}${GTATOOLS_VERSION_SUFFIX}-r${GTATOOLS_VERSION_SVNREV}")
ELSE(GTATOOLS_VERSION_SVNREV)
    SET(GTATOOLS_VERSION "${GTATOOLS_VERSION_MAJOR}.${GTATOOLS_VERSION_MINOR}${GTATOOLS_VERSION_SUFFIX}")
ENDIF(GTATOOLS_VERSION_SVNREV)


SET(LIBGTAFORMATS_BUILD ON CACHE BOOL "Whether you want to build libgtaformats")
SET(LIBGTA_BUILD ON CACHE BOOL "Whether you want to build libgta")
SET(GTAIMG_BUILD ON CACHE BOOL "Whether you want to build gtaimg")
SET(GTATXD_BUILD ON CACHE BOOL "Whether you want to build gtatxd")
SET(GTATOOLS_GUI_BUILD ON CACHE BOOL "Whether you want to build gtatools-gui")
SET(GTATOOLS_TEST_BUILD OFF CACHE BOOL "Whether you want to build gtatools-test")
SET(GTAFORMATS_TEST_BUILD OFF CACHE BOOL "Whether you want to build gtaformats-test")

IF(GTATOOLS_GUI_BUILD)
    IF(CPACK_PACKAGE_EXECUTABLES)
        SET(CPACK_PACKAGE_EXECUTABLES "${CPACK_PACKAGE_EXECUTABLES};")
    ENDIF(CPACK_PACKAGE_EXECUTABLES)
	SET(CPACK_PACKAGE_EXECUTABLES "${CPACK_PACKAGE_EXECUTABLES}gtatools-gui;GTATools GUI Viewer")
ENDIF(GTATOOLS_GUI_BUILD)

IF(GTATOOLS_TEST_BUILD)
    IF(CPACK_PACKAGE_EXECUTABLES)
        SET(CPACK_PACKAGE_EXECUTABLES "${CPACK_PACKAGE_EXECUTABLES};")
    ENDIF(CPACK_PACKAGE_EXECUTABLES)
	SET(CPACK_PACKAGE_EXECUTABLES "${CPACK_PACKAGE_EXECUTABLES}gtatools-test;GTATools Test Program")
ENDIF(GTATOOLS_TEST_BUILD)

INCLUDE(CPack)

SET(ADDITIONAL_INSTALL_FILES "" CACHE STRING "Paths to files which will be additionally installed to the bin subdirectory.")
SET(ADDITIONAL_INSTALL_FILES_ENABLED OFF CACHE BOOL "Set true to enable ADDITIONAL_INSTALL_FILES")


IF(NXCOMMON_BUILDING)
    MESSAGE(STATUS "Building integrated nxcommon.")
    ADD_SUBDIRECTORY(nxcommon/src nxcommon/build)
ENDIF()

IF(NOT TARGET nxcommon)
    FIND_PACKAGE(Nxcommon REQUIRED CONFIG)
ENDIF()

INCLUDE(NxUtilities)

GLOBAL_ADD_MODULE_PATH(GTATOOLS "${CMAKE_CURRENT_SOURCE_DIR}/cmakemodules")

IF(LIBGTAFORMATS_BUILD)
    ADD_SUBDIRECTORY(libgtaformats/src libgtaformats/build)
ENDIF()

IF(LIBGTA_BUILD)
    ADD_SUBDIRECTORY(libgta/src libgta/build)
ENDIF()

IF(GTAIMG_BUILD)
    ADD_SUBDIRECTORY(gtaimg/src gtaimg/build)
ENDIF()

IF(GTATXD_BUILD)
    ADD_SUBDIRECTORY(gtatxd/src gtatxd/build)
ENDIF()

IF(GTATOOLS_GUI_BUILD)
    ADD_SUBDIRECTORY(gtatools-gui/src gtatools-gui/build)
ENDIF()

IF(GTATOOLS_TEST_BUILD)
    ADD_SUBDIRECTORY(gtatools-test/src gtatools-test/build)
ENDIF()

IF(GTAFORMATS_TEST_BUILD)
    ADD_SUBDIRECTORY(gtaformats-test/src gtaformats-test/build)
ENDIF()


IF(ADDITIONAL_INSTALL_FILES_ENABLED AND ADDITIONAL_INSTALL_FILES)
    INSTALL(FILES ${ADDITIONAL_INSTALL_FILES} DESTINATION bin)
ENDIF(ADDITIONAL_INSTALL_FILES_ENABLED AND ADDITIONAL_INSTALL_FILES)

