# Copyright 2010-2014 David "Alemarius Nexus" Lerch
# 
# This file is part of gtaformats.
#
# gtaformats is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# gtaformats is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.
#
# Additional permissions are granted, which are listed in the file
# GPLADDITIONS.


CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(prj-gtaformats C CXX)

CMAKE_POLICY(SET CMP0028 NEW)

INCLUDE(TestBigEndian)

SET(GTATOOLS_INSTALL_ENABLED OFF CACHE BOOL "If true, CMake INSTALL commands will be run for gtatools.")

SET(GTAFORMATS_ENABLE_PVRTEXLIB OFF CACHE BOOL "Whether we use the proprietary PVRTexLib for PVRTC-compressed textures (a gtatools extension).")
SET(GTAFORMATS_ENABLE_SQUISH ON CACHE BOOL "Whether we use libsquish for DXT-compressed textures. If false, those textures are not supported directly by the library")

TEST_BIG_ENDIAN(GTAFORMATS_BIG_ENDIAN)

IF(NOT TARGET nxcommon)
    FIND_PACKAGE(Nxcommon REQUIRED CONFIG)
ENDIF()

INCLUDE(NxUtilities)

GLOBAL_ADD_MODULE_PATH(GTATOOLS "${CMAKE_CURRENT_SOURCE_DIR}/../../cmakemodules")

ADD_SUBDIRECTORY(gtaformats)

IF(GTAFORMATS_ENABLE_SQUISH)
    FIND_PACKAGE(Libsquish)
ENDIF()

IF(GTAFORMATS_ENABLE_PVRTEXLIB)
    FIND_PACKAGE(PVRTexLib)
ENDIF()

FIND_PACKAGE(Bullet COMPONENTS BulletCollision BulletDynamics LinearMath)

IF(NXCOMMON_QT_SUPPORT STREQUAL "qt4")
    FIND_PACKAGE(Qt4 4.6.0 COMPONENTS QtCore REQUIRED)
    INCLUDE(${QT_USE_FILE})
ELSEIF(NXCOMMON_QT_SUPPORT STREQUAL "qt5")
    FIND_PACKAGE(Qt5Core)
ENDIF()

IF(APPLE)
    FIND_PATH(ICONV_INCLUDE_DIRS iconv.h)
    FIND_LIBRARY(ICONV_LIB_PATH NAMES iconv libiconv)
ENDIF(APPLE)

SET(LIBRARIES "")
SET(INCLUDES "")

IF(GTAFORMATS_ENABLE_SQUISH)
    SET(LIBRARIES ${LIBRARIES} "${LIBSQUISH_LIBRARIES}")
    SET(INCLUDES ${INCLUDES} "${LIBSQUISH_INCLUDE_DIRS}")
ENDIF()
IF(GTAFORMATS_ENABLE_PVRTEXLIB)
    SET(LIBRARIES ${LIBRARIES} "${PVRTEXLIB_LIBRARIES}")
    SET(INCLUDES ${INCLUDES} "${PVRTEXLIB_INCLUDE_DIRS}")
ENDIF()

IF(APPLE)
    SET(LIBRARIES ${LIBRARIES} "${ICONV_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${ICONV_INCLUDE_PATH}")
ENDIF(APPLE)

SET(LIBRARIES ${LIBRARIES} ${BULLET_LIBRARIES})
SET(INCLUDES ${INCLUDES} ${BULLET_INCLUDE_DIRS})


ADD_LIBRARY(gtaformats STATIC ${SRCS})
TARGET_LINK_LIBRARIES(gtaformats PUBLIC ${LIBRARIES} nxcommon)
TARGET_INCLUDE_DIRECTORIES(gtaformats PUBLIC ${INCLUDES} $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/libgtaformats/build> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
TARGET_ADD_INSTALL_STATE_DEFINE(gtaformats GTAFORMATS)

SET(GTATOOLS_CMAKE_MODULE_DIR_BUILD "${CMAKE_CURRENT_SOURCE_DIR}/../../cmakemodules")
SET(GTATOOLS_CMAKE_MODULE_DIR_INSTALL ".") # Relative to the export file

IF(GTATOOLS_INSTALL_ENABLED)
    INSTALL(TARGETS gtaformats EXPORT LibgtaformatsTargets DESTINATION lib)
    INSTALL(	DIRECTORY gtaformats
            DESTINATION include
            FILES_MATCHING
                PATTERN *.h
                PATTERN .svn EXCLUDE
                PATTERN *.cmake.h EXCLUDE
        )
    INSTALL(    FILES "${CMAKE_CURRENT_BINARY_DIR}/gtaformats/config.h"
            DESTINATION "include/gtaformats"
        )

    EXPORT (
        EXPORT LibgtaformatsTargets
        FILE "${CMAKE_CURRENT_BINARY_DIR}/cmakecfg/LibgtaformatsTargets.cmake"
    )

    CONFIGURE_FILE(LibgtaformatsConfig.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/cmakecfg/LibgtaformatsConfig.cmake" @ONLY)

    INSTALL (
        EXPORT LibgtaformatsTargets
        FILE LibgtaformatsTargets.cmake
        DESTINATION lib/cmake
    )

    INSTALL (
        FILES "${CMAKE_CURRENT_BINARY_DIR}/cmakecfg/LibgtaformatsConfig.cmake"
        DESTINATION lib/cmake
    )
ENDIF()
