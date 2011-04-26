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
PROJECT(prj-gta CXX)

ADD_SUBDIRECTORY(gta)

IF(prj-gtaformats_SOURCE_DIR)
    SET(GTAFORMATS_INCLUDE_PATH "${prj-gtaformats_SOURCE_DIR}" CACHE PATH "Path to the gtaformats include dir (EXCLUDING the 'gtaformats' part)")
ELSE(prj-gtaformats_SOURCE_DIR)
    SET(GTAFORMATS_INCLUDE_PATH "" CACHE PATH "Path to the gtaformats include dir (EXCLUDING the 'gtaformats' part)")
ENDIF(prj-gtaformats_SOURCE_DIR)

IF(WIN32)
    SET(GL_DEFAULT_LIB opengl32)
	SET(GLU_DEFAULT_LIB glu32)
	SET(GLEW_DEFAULT_LIB glew32)
ELSE(WIN32)
    SET(GL_DEFAULT_LIB GL)
	SET(GLU_DEFAULT_LIB GLU)
	SET(GLEW_DEFAULT_LIB GLEW)
ENDIF(WIN32)

SET(GTA_ENABLE_PVRTEXLIB OFF CACHE BOOL "Whether we use the proprietary PVRTexLib for PVRTC-compressed textures.")
SET(GTA_ENABLE_SQUISH ON CACHE BOOL "If enabled, libsquish will be available to decompress DXT-textures.")
SET(GTA_USE_OPENGL_ES OFF CACHE BOOL "If enabled, OpenGL ES will be used instead of Desktop OpenGL.")

SET(GTAFORMATS_LIB_PATH "gtaformats" CACHE FILEPATH "Path to (or name of) libgtaformats library file")
SET(SQUISH_INCLUDE_PATH "" CACHE PATH "Directory containing libsquish header files.")
SET(SQUISH_LIB_PATH "squish" CACHE FILEPATH "Path to (or name of) libsquish library file")
SET(PVRTEXLIB_INCLUDE_PATH "" CACHE PATH "Directory containing PVRTexLib header files.")
SET(PVRTEXLIB_LIB_PATH "PVRTexLib" CACHE FILEPATH "Path to (or name of) PVRTexLib library file.")
SET(BULLET_DYNAMICS_LIB_PATH "BulletDynamics" CACHE FILEPATH "Path to (or name of) the BulletDynamics library file")
SET(BULLET_COLLISION_LIB_PATH "BulletCollision" CACHE FILEPATH "Path to (or name of) the BulletCollision library file")
SET(BULLET_LINEAR_MATH_LIB_PATH "LinearMath" CACHE FILEPATH "Path to (or name of) the Bullet LinearMath library file")
SET(BULLET_INCLUDE_PATH "" CACHE PATH "Directory containing Bullet header files")
SET(BOOST_INCLUDE_PATH "" CACHE PATH "Directory to the boost header files.")

IF(GTA_USE_OPENGL_ES)
    SET(GLESV2_INCLUDE_PATH "" CACHE PATH "Directory containing the OpenGL ES 2.0 header file(s)")
    SET(GLESV2_LIB_PATH "GLESv2" CACHE FILEPATH "Path to (or name of) the OpenGL ES 2.0 library file")
ELSE(GTA_USE_OPENGL_ES)
    SET(GL_INCLUDE_PATH "" CACHE PATH "Directory containing the OpenGL header file(s)")
    SET(GL_LIB_PATH "${GL_DEFAULT_LIB}" CACHE FILEPATH "Path to (or name of) the OpenGL library file")
    SET(GLU_INCLUDE_PATH "" CACHE PATH "Directory containing the OpenGL Utility (GLU) header file(s)")
    SET(GLU_LIB_PATH "${GLU_DEFAULT_LIB}" CACHE FILEPATH "Path to (or name of) the OpenGL Utility (GLU) library file")
    SET(GLEW_INCLUDE_PATH "" CACHE PATH "Directory containing the OpenGL Extension Wrangler (GLEW) header file(s)")
    SET(GLEW_LIB_PATH "${GLEW_DEFAULT_LIB}" CACHE FILEPATH "Path to (or name of) the OpenGL Extension Wrangler (GLEW) library file")
ENDIF(GTA_USE_OPENGL_ES)

IF(NOT SQUISH_LIB_PATH)
	SET(SQUISH_LIB_PATH squish)
ENDIF(NOT SQUISH_LIB_PATH)

IF(GTA_USE_OPENGL_ES)
    SET(GL_LIBRARIES "${GLESV2_LIB_PATH}")
    SET(GL_INCDIRS "${GLESV2_INCLUDE_PATH}")
ELSE(GTA_USE_OPENGL_ES)
    SET(GL_LIBRARIES "${GL_LIB_PATH}" "${GLU_LIB_PATH}" "${GLEW_LIB_PATH}")
    SET(GL_INCDIRS "${GL_INCLUDE_PATH}" "${GLU_INCLUDE_PATH}" "${GLEW_INCLUDE_PATH}")
ENDIF(GTA_USE_OPENGL_ES)

INCLUDE_DIRECTORIES(${SQUISH_INCLUDE_PATH} ${PVRTECLIB_INCLUDE_PATH} ${GTAFORMATS_INCLUDE_PATH} ${GL_INCDIRS}
        ${BULLET_INCLUDE_PATH} ${BOOST_INCLUDE_PATH})

ADD_LIBRARY(gta STATIC ${SRCS})
TARGET_LINK_LIBRARIES(gta ${SQUISH_LIB_PATH} ${PVRTEXLIB_LIB_PATH} ${GTAFORMATS_LIB_PATH} ${GL_LIBRARIES}
        ${BULLET_DYNAMICS_LIB_PATH} ${BULLET_COLLISION_LIB_PATH} ${BULLET_LINEAR_MATH_LIB_PATH})

INSTALL(TARGETS gta DESTINATION lib)
INSTALL(	DIRECTORY gta
		DESTINATION include
		FILES_MATCHING
			PATTERN *.h
			PATTERN .svn EXCLUDE
	)


