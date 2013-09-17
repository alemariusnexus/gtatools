# Copyright 2010-2013 David "Alemarius Nexus" Lerch
# 
# This file is part of gtaformats-test.
#
# gtaformats-test is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# gtaformats-test is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with gtaformats-test.  If not, see <http://www.gnu.org/licenses/>.

# Additional permissions are granted, which are listed in the file
# GPLADDITIONS.


CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(prj-gtaformats-test C CXX)

ADD_SUBDIRECTORY(src)

IF(prj-gtaformats_SOURCE_DIR)
    SET(GTAFORMATS_LIB_PATH gtaformats)
    SET(GTAFORMATS_INCLUDE_PATH "${prj-gtaformats_SOURCE_DIR}" "${prj-gtaformats_BINARY_DIR}" "${prj-gtaformats_SOURCE_DIR}/gtaformats")
ELSE(prj-gtaformats_SOURCE_DIR)
    FIND_LIBRARY(GTAFORMATS_LIB_PATH NAMES gtaformats libgtaformats)
    FIND_PATH(GTAFORMATS_INCLUDE_PATH gtaformats/config.h)
ENDIF(prj-gtaformats_SOURCE_DIR)

FIND_PACKAGE(Boost)
FIND_PACKAGE(GTest)
FIND_PACKAGE(PNG)

FIND_PATH(SQUISH_INCLUDE_PATH squish.h)
FIND_LIBRARY(SQUISH_LIB_PATH NAMES squish libsquish)

FIND_PATH(PVRTEXLIB_INCLUDE_PATH PVRTexLib.h)
FIND_LIBRARY(PVRTEXLIB_LIB_PATH NAMES PVRTexLib libPVRTexLib)

IF(APPLE)
    FIND_PATH(ICONV_INCLUDE_PATH iconv.h)
    FIND_LIBRARY(ICONV_LIB_PATH NAMES iconv libiconv)
ENDIF(APPLE)


SET(LIBRARIES "")
SET(INCLUDES "")

SET(LIBRARIES ${LIBRARIES} "${GTAFORMATS_LIB_PATH}")
SET(INCLUDES ${INCLUDES} ${GTAFORMATS_INCLUDE_PATH})

SET(INCLUDES ${INCLUDES} "${Boost_INCLUDE_DIRS}")

SET(LIBRARIES ${LIBRARIES} ${GTEST_LIBRARIES})
SET(INCLUDES ${INCLUDES} ${GTEST_INCLUDE_DIRS})

SET(LIBRARIES ${LIBRARIES} "${PNG_LIBRARIES}")
SET(INCLUDES ${INCLUDES} "${PNG_INCLUDE_DIR}")

IF(GTAFORMATS_ENABLE_SQUISH)
    SET(LIBRARIES ${LIBRARIES} "${SQUISH_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${SQUISH_INCLUDE_PATH}")
ENDIF(GTAFORMATS_ENABLE_SQUISH)

IF(GTAFORMATS_ENABLE_PVRTEXLIB)
    SET(LIBRARIES ${LIBRARIES} "${PVRTEXLIB_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${PVRTEXLIB_INCLUDE_PATH}")
ENDIF(GTAFORMATS_ENABLE_PVRTEXLIB)

IF(APPLE)
    SET(LIBRARIES ${LIBRARIES} "${ICONV_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${ICONV_INCLUDE_PATH}")
ENDIF(APPLE)

INCLUDE_DIRECTORIES(${INCLUDES})

ADD_EXECUTABLE(gtaformats-test ${SRCS})
TARGET_LINK_LIBRARIES(gtaformats-test ${LIBRARIES})

INSTALL(TARGETS gtaformats-test DESTINATION bin)

