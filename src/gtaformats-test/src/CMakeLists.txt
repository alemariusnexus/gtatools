# Copyright 2010-2014 David "Alemarius Nexus" Lerch
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

CMAKE_POLICY(SET CMP0020 OLD)

ADD_SUBDIRECTORY(src)

IF(NOT TARGET nxcommon)
    FIND_PACKAGE(Libnxcommon REQUIRED CONFIG)
ENDIF()

IF(NOT TARGET gtaformats)
    FIND_PACKAGE(Libgtaformats REQUIRED CONFIG)
ENDIF()

FIND_PACKAGE(GTest REQUIRED)
FIND_PACKAGE(PNG REQUIRED)

IF(GTAFORMATS_ENABLE_SQUISH)
    FIND_PACKAGE(Libsquish REQUIRED)
ENDIF()

IF(GTAFORMATS_ENABLE_PVRTEXLIB)
    FIND_PACKAGE(PVRTexLib REQUIRED)
ENDIF()

IF(NXCOMMON_QT_SUPPORT STREQUAL "qt4")
    FIND_PACKAGE(Qt4 4.6.0 COMPONENTS QtCore REQUIRED)
    INCLUDE(${QT_USE_FILE})
ELSEIF(NXCOMMON_QT_SUPPORT STREQUAL "qt5")
    FIND_PACKAGE(Qt5Core)
ENDIF()

IF(APPLE)
    FIND_PATH(ICONV_INCLUDE_DIRS iconv.h)
    FIND_LIBRARY(ICONV_LIBRARIES NAMES iconv libiconv)
ENDIF(APPLE)


SET(LIBRARIES nxcommon gtaformats)
SET(INCLUDES "")

SET(LIBRARIES ${LIBRARIES} ${GTEST_LIBRARIES})
SET(INCLUDES ${INCLUDES} ${GTEST_INCLUDE_DIRS})

SET(LIBRARIES ${LIBRARIES} "${PNG_LIBRARIES}")
SET(INCLUDES ${INCLUDES} "${PNG_INCLUDE_DIR}")

IF(GTAFORMATS_ENABLE_SQUISH)
    SET(LIBRARIES ${LIBRARIES} "${SQUISH_LIBRARIES}")
    SET(INCLUDES ${INCLUDES} "${SQUISH_INCLUDE_DIRS}")
ENDIF(GTAFORMATS_ENABLE_SQUISH)

IF(GTAFORMATS_ENABLE_PVRTEXLIB)
    SET(LIBRARIES ${LIBRARIES} "${PVRTEXLIB_LIBRARIES}")
    SET(INCLUDES ${INCLUDES} "${PVRTEXLIB_INCLUDE_DIRS}")
ENDIF(GTAFORMATS_ENABLE_PVRTEXLIB)

IF(APPLE)
    SET(LIBRARIES ${LIBRARIES} "${ICONV_LIBRARIES}")
    SET(INCLUDES ${INCLUDES} "${ICONV_INCLUDE_DIRS}")
ENDIF(APPLE)

IF(UNIX)
    SET(CMAKE_THREAD_PREFER_PTHREAD ON)
    FIND_PACKAGE(Threads)
    
    SET(LIBRARIES ${LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
ENDIF()

ADD_EXECUTABLE(gtaformats-test ${SRCS})
TARGET_LINK_LIBRARIES(gtaformats-test PUBLIC ${LIBRARIES})
TARGET_INCLUDE_DIRECTORIES(gtaformats-test PUBLIC ${INCLUDES})

INSTALL(TARGETS gtaformats-test DESTINATION bin)

