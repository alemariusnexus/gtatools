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
PROJECT(prj-gtaformats C CXX)


SET(GTAFORMATS_ENABLE_PVRTEXLIB OFF CACHE BOOL "Whether we use the proprietary PVRTexLib for PVRTC-compressed textures.")
SET(GTAFORMATS_ENABLE_SQUISH ON CACHE BOOL "Whether we use libsquish for DXT-compressed textures. If false, those textures are not supported directly by the library")
SET(EXCEPTION_POSITION_INFO OFF CACHE BOOL "If true, exception messages will include the full path to the source file and the line in which the exception was thrown.")


ADD_SUBDIRECTORY(gtaformats)

FIND_PATH(SQUISH_INCLUDE_PATH squish.h)
FIND_LIBRARY(SQUISH_LIB_PATH NAMES squish libsquish)

FIND_PATH(PVRTEXLIB_INCLUDE_PATH PVRTexLib.h)
FIND_LIBRARY(PVRTEXLIB_LIB_PATH NAMES PVRTexLib libPVRTexLib)

FIND_PACKAGE(Boost)

SET(LIBRARIES "")
SET(INCLUDES "")

IF(GTAFORMATS_ENABLE_SQUISH)
    SET(LIBRARIES ${LIBRARIES} "${SQUISH_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${SQUISH_INCLUDE_PATH}")
ENDIF(GTAFORMATS_ENABLE_SQUISH)
IF(GTAFORMATS_ENABLE_PVRTEXLIB)
    SET(LIBRARIES ${LIBRARIES} "${PVRTEXLIB_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${PVRTEXLIB_INCLUDE_PATH}")
ENDIF(GTAFORMATS_ENABLE_PVRTEXLIB)

SET(INCLUDES ${INCLUDES} "${Boost_INCLUDE_DIRS}")

# The directory containing the generated config.h
SET(INCLUDES ${INCLUDES} "${CMAKE_BINARY_DIR}/libgtaformats/build")

INCLUDE_DIRECTORIES(${INCLUDES})

ADD_LIBRARY(gtaformats STATIC ${SRCS})

IF(GTAFORMATS_USE_SQUISH)
    TARGET_LINK_LIBRARIES(gtaformats ${LIBRARIES})
ENDIF(GTAFORMATS_USE_SQUISH)

INSTALL(TARGETS gtaformats DESTINATION lib)
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


