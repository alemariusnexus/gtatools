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
PROJECT(prj-gta CXX)

ADD_SUBDIRECTORY(gta)

IF(prj-gtaformats_SOURCE_DIR)
    SET(GTAFORMATS_INCLUDE_PATH "${prj-gtaformats_SOURCE_DIR}" CACHE PATH "Path to the gtaformats include dir (EXCLUDING the 'gtaformats' part)")
ELSE(prj-gtaformats_SOURCE_DIR)
    SET(GTAFORMATS_INCLUDE_PATH "" CACHE PATH "Path to the gtaformats include dir (EXCLUDING the 'gtaformats' part)")
ENDIF(prj-gtaformats_SOURCE_DIR)

IF(UNIX)
	SET(GL_DEFAULT_LIB GL)
	SET(GLU_DEFAULT_LIB GLU)
	SET(GLEW_DEFAULT_LIB GLEW)
ELSE(UNIX) # Windows...
	SET(GL_DEFAULT_LIB opengl32)
	SET(GLU_DEFAULT_LIB glu32)
	SET(GLEW_DEFAULT_LIB glew32)
ENDIF(UNIX)

SET(GTAFORMATS_LIB_PATH "gtaformats" CACHE PATH "Path to (or name of) libgtaformats library file")
SET(SQUISH_INCLUDE_PATH "" CACHE PATH "Directory containing libsquish header files.")
SET(SQUISH_LIB_PATH "squish" CACHE PATH "Path to (or name of) libsquish library file")
SET(GL_INCLUDE_PATH "" CACHE PATH "Directory containing the OpenGL header file(s)")
SET(GL_LIB_PATH "${GL_DEFAULT_LIB}" CACHE PATH "Path to (or name of) the OpenGL library file")
SET(GLU_INCLUDE_PATH "" CACHE PATH "Directory containing the OpenGL Utility (GLU) header file(s)")
SET(GLU_LIB_PATH "${GLU_DEFAULT_LIB}" CACHE PATH "Path to (or name of) the OpenGL Utility (GLU) library file")
SET(GLEW_INCLUDE_PATH "" CACHE PATH "Directory containing the OpenGL Extension Wrangler (GLEW) header file(s)")
SET(GLEW_LIB_PATH "${GLEW_DEFAULT_LIB}" CACHE PATH "Path to (or name of) the OpenGL Extension Wrangler (GLEW) library file")

SET(GTA_ENABLE_SQUISH ON CACHE BOOL "If enabled, libsquish will be available to decompress DXT-textures.")

IF(NOT SQUISH_LIB_PATH)
	SET(SQUISH_LIB_PATH squish)
ENDIF(NOT SQUISH_LIB_PATH)

INCLUDE_DIRECTORIES(${SQUISH_INCLUDE_PATH} ${GTAFORMATS_INCLUDE_PATH} ${GL_INCLUDE_PATH} ${GLU_INCLUDE_PATH} ${GLEW_INCLUDE_PATH})

ADD_LIBRARY(gta STATIC ${SRCS})
TARGET_LINK_LIBRARIES(gta ${SQUISH_LIB_PATH} ${GTAFORMATS_LIB_PATH} ${GL_LIB_PATH} ${GLU_LIB_PATH} ${GLEW_LIB_PATH})

INSTALL(TARGETS gta DESTINATION lib)
INSTALL(	DIRECTORY gta
		DESTINATION include
		FILES_MATCHING
			PATTERN *.h
			PATTERN .svn EXCLUDE
	)


