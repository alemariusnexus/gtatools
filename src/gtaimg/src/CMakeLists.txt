# Copyright 2010-2014 David "Alemarius Nexus" Lerch
# 
# This file is part of gtaimg.
#
# gtaimg is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# gtaimg is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with gtaimg.  If not, see <http://www.gnu.org/licenses/>.

# Additional permissions are granted, which are listed in the file
# GPLADDITIONS.


CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(prj-gtaimg C CXX)

CMAKE_POLICY(SET CMP0020 NEW)
CMAKE_POLICY(SET CMP0028 NEW)

SET(GTATOOLS_INSTALL_ENABLED OFF CACHE BOOL "If true, CMake INSTALL commands will be run for gtatools.")

ADD_SUBDIRECTORY(src)

IF(NOT TARGET nxcommon)
    FIND_PACKAGE(Libnxcommon REQUIRED CONFIG)
ENDIF()

IF(NOT TARGET gtaformats)
    FIND_PACKAGE(Libgtaformats REQUIRED CONFIG)
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


SET(LIBRARIES "")
SET(INCLUDES "")

IF(APPLE)
    SET(LIBRARIES ${LIBRARIES} "${ICONV_LIBRARIES}")
    SET(INCLUDES ${INCLUDES} "${ICONV_INCLUDE_DIRS}")
ENDIF(APPLE)

ADD_EXECUTABLE(gtaimg ${SRCS})
TARGET_LINK_LIBRARIES(gtaimg PUBLIC ${LIBRARIES} nxcommon gtaformats)
TARGET_INCLUDE_DIRECTORIES(gtaimg PUBLIC ${INCLUDES})


IF(GTATOOLS_INSTALL_ENABLED)
    INSTALL(TARGETS gtaimg DESTINATION bin)
ENDIF()

