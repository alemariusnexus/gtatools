# Copyright 2010-2011 David "Alemarius Nexus" Lerch
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
PROJECT(prj-gtaimg CXX)

ADD_SUBDIRECTORY(src)

IF(prj-gtaformats_SOURCE_DIR)
    SET(GTAFORMATS_INCLUDE_PATH "${prj-gtaformats_SOURCE_DIR}" CACHE PATH "Path to the gtaformats include dir (EXCLUDING the 'gtaformats' part)")
ELSE(prj-gtaformats_SOURCE_DIR)
    SET(GTAFORMATS_INCLUDE_PATH "" CACHE PATH "Path to the gtaformats include dir (EXCLUDING the 'gtaformats' part)")
ENDIF(prj-gtaformats_SOURCE_DIR)
    
SET(GTAFORMATS_LIB_PATH "gtaformats" CACHE FILEPATH "Path to (or name of) the libgtaformats library file")

IF(NOT GTAFORMATS_LIB_PATH)
	SET(GTAFORMATS_LIB_PATH gtaformats)
ENDIF(NOT GTAFORMATS_LIB_PATH)

INCLUDE_DIRECTORIES(${GTAFORMATS_INCLUDE_PATH} ${CMAKE_BINARY_DIR})

ADD_EXECUTABLE(gtaimg ${SRCS})
TARGET_LINK_LIBRARIES(gtaimg ${GTAFORMATS_LIB_PATH})

INSTALL(TARGETS gtaimg DESTINATION bin)

