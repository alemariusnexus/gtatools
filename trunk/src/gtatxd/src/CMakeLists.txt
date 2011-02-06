# Copyright 2010 David "Alemarius Nexus" Lerch
# 
# This file is part of gtatxd.
#
# gtatxd is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# gtatxd is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with gtatxd.  If not, see <http://www.gnu.org/licenses/>.


CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(prj-gtatxd CXX)

ADD_SUBDIRECTORY(src)

IF(prj-gtaformats_SOURCE_DIR)
    SET(GTAFORMATS_INCLUDE_PATH "${prj-gtaformats_SOURCE_DIR}" CACHE PATH "Path to the gtaformats include dir (EXCLUDING the 'gtaformats' part)")
ELSE(prj-gtaformats_SOURCE_DIR)
    SET(GTAFORMATS_INCLUDE_PATH "" CACHE PATH "Path to the gtaformats include dir (EXCLUDING the 'gtaformats' part)")
ENDIF(prj-gtaformats_SOURCE_DIR)

SET(GTAFORMATS_LIB_PATH "gtaformats" CACHE FILEPATH "Path to (or name of) the libgtaformats library file")
SET(SQUISH_INCLUDE_PATH "" CACHE PATH "Directory containing libsquish header files.")
SET(SQUISH_LIB_PATH "squish" CACHE FILEPATH "Path to (or name of) libsquish library file")
SET(PNG_INCLUDE_PATH "" CACHE PATH "Path to the libpng include dir")
SET(PNG_LIB_PATH "png" CACHE FILEPATH "Path to (or name of) the libpng library file")
SET(Z_INCLUDE_PATH "" CACHE PATH "Path to the zlib include dir")
SET(Z_LIB_PATH "png" CACHE FILEPATH "Path to (or name of) the zlib library file")

IF(NOT GTAFORMATS_LIB_PATH)
	SET(GTAFORMATS_LIB_PATH gtaformats)
ENDIF(NOT GTAFORMATS_LIB_PATH)
IF(NOT SQUISH_LIB_PATH)
	SET(SQUISH_LIB_PATH squish)
ENDIF(NOT SQUISH_LIB_PATH)
IF(NOT PNG_LIB_PATH)
	SET(PNG_LIB_PATH png)
ENDIF(NOT PNG_LIB_PATH)
IF(NOT Z_LIB_PATH)
	SET(Z_LIB_PATH png)
ENDIF(NOT Z_LIB_PATH)

INCLUDE_DIRECTORIES(${GTAFORMATS_INCLUDE_PATH} ${SQUISH_INCLUDE_PATH} ${PNG_INCLUDE_PATH} ${Z_INCLUDE_PATH}
         ${CMAKE_BINARY_DIR})

ADD_EXECUTABLE(gtatxd ${SRCS})
TARGET_LINK_LIBRARIES(gtatxd ${GTAFORMATS_LIB_PATH} ${SQUISH_LIB_PATH} ${PNG_LIB_PATH} ${Z_LIB_PATH})

INSTALL(TARGETS gtatxd DESTINATION bin)

