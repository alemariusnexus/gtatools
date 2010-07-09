# Copyright 2010 David "Alemarius Nexus" Lerch
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


CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(prj-gtaformats CXX)


SET(GF_USE_SQUISH ON CACHE BOOL "Whether we use libsquish for DXT-compressed textures. If false, those textures are not supported.")


ADD_SUBDIRECTORY(gtaformats)

SET(SQUISH_INCLUDE_PATH "" CACHE PATH "Directory containing libsquish header files.")
SET(SQUISH_LIB_PATH "squish" CACHE PATH "Path to (or name of) libsquish library file")

IF(NOT SQUISH_LIB_PATH)
	SET(SQUISH_LIB_PATH squish)
ENDIF(NOT SQUISH_LIB_PATH)

INCLUDE_DIRECTORIES(${SQUISH_INCLUDE_PATH} ${CMAKE_BINARY_DIR})

ADD_LIBRARY(gtaformats STATIC ${SRCS})
TARGET_LINK_LIBRARIES(gtaformats ${SQUISH_LIB_PATH})

INSTALL(TARGETS gtaformats DESTINATION lib)
INSTALL(	DIRECTORY gtaformats
		DESTINATION include
		FILES_MATCHING
			PATTERN *.h
			PATTERN .svn EXCLUDE
	)


