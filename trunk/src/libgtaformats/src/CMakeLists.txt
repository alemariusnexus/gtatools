# Copyright 2010-2011 David "Alemarius Nexus" Lerch
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
# Additional permissions are granted, which are listed in the file
# GPLADDITIONS.


CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(prj-gtaformats CXX)


SET(GTAFORMATS_ENABLE_PVRTEXLIB OFF CACHE BOOL "Whether we use the proprietary PVRTexLib for PVRTC-compressed textures.")
SET(GTAFORMATS_ENABLE_SQUISH ON CACHE BOOL "Whether we use libsquish for DXT-compressed textures. If false, those textures are not supported directly by the library")
SET(EXCEPTION_POSITION_INFO OFF CACHE BOOL "If true, exception messages will include the full path to the source file and the line in which the exception was thrown.")


ADD_SUBDIRECTORY(gtaformats)

SET(SQUISH_INCLUDE_PATH "" CACHE PATH "Directory containing libsquish header files.")
SET(SQUISH_LIB_PATH "squish" CACHE FILEPATH "Path to (or name of) libsquish library file.")

SET(PVRTEXLIB_INCLUDE_PATH "" CACHE PATH "Directory containing PVRTexLib header files.")
SET(PVRTEXLIB_LIB_PATH "PVRTexLib" CACHE FILEPATH "Path to (or name of) PVRTexLib library file.")

INCLUDE_DIRECTORIES(${SQUISH_INCLUDE_PATH} ${PVRTEXLIB_INCLUDE_PATH}
        "${CMAKE_BINARY_DIR}/libgtaformats/build/gtaformats")

ADD_LIBRARY(gtaformats STATIC ${SRCS})

IF(GTAFORMATS_USE_SQUISH)
    TARGET_LINK_LIBRARIES(gtaformats ${SQUISH_LIB_PATH} ${PVRTEXLIB_LIB_PATH})
ENDIF(GTAFORMATS_USE_SQUISH)

INSTALL(TARGETS gtaformats DESTINATION lib)
INSTALL(	DIRECTORY gtaformats
		DESTINATION include
		FILES_MATCHING
			PATTERN *.h
			PATTERN .svn EXCLUDE
	)


