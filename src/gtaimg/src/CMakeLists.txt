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


CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(prj-gtaimg C CXX)

ADD_SUBDIRECTORY(src)

FIND_PACKAGE(Libnxcommon)
FIND_PACKAGE(Libgtaformats)

IF(APPLE)
    FIND_PATH(ICONV_INCLUDE_PATH iconv.h)
    FIND_LIBRARY(ICONV_LIB_PATH NAMES iconv libiconv)
ENDIF(APPLE)


SET(LIBRARIES "")
SET(INCLUDES "")

SET(LIBRARIES ${LIBRARIES} "${LIBGTAFORMATS_LIBRARIES}")
SET(INCLUDES ${INCLUDES} ${LIBGTAFORMATS_INCLUDE_DIRS})

SET(LIBRARIES ${LIBRARIES} "${LIBNXCOMMON_LIBRARIES}")
SET(INCLUDES ${INCLUDES} ${LIBNXCOMMON_INCLUDE_DIRS})

IF(APPLE)
    SET(LIBRARIES ${LIBRARIES} "${ICONV_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${ICONV_INCLUDE_PATH}")
ENDIF(APPLE)

INCLUDE_DIRECTORIES(${INCLUDES})

ADD_EXECUTABLE(gtaimg ${SRCS})
TARGET_LINK_LIBRARIES(gtaimg ${LIBRARIES})

INSTALL(TARGETS gtaimg DESTINATION bin)

