# Copyright 2010-2014 David "Alemarius Nexus" Lerch
# 
# This file is part of gtatools-test.
#
# gtatools-test is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# gtatools-test is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with gtatools-test.  If not, see <http://www.gnu.org/licenses/>.

# Additional permissions are granted, which are listed in the file
# GPLADDITIONS.

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(gtatools-test C CXX)

INCLUDE(../../ResourceCompiler.cmake)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "gtatools-test - A test program for gtatools")
SET(CPACK_PACKAGE_VENDOR "David 'Alemarius Nexus' Lerch")
#SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_SOURCE_IGNORE_FILES "\\\\.svn;.*~")

INCLUDE(CPack)
INCLUDE(CheckSymbolExists)

SET(GT_GTASA_PATH "" CACHE PATH "Path to the GTA San Andreas root directory")
SET(GT_WINDOW_WIDTH "512" CACHE STRING "Width in pixels of the GL window")
SET(GT_WINDOW_HEIGHT "512" CACHE STRING "Height in pixels of the GL window")
SET(GT_WINDOW_POS_X "0" CACHE STRING "X position of the GL window")
SET(GT_WINDOW_POS_Y "0" CACHE STRING "X position of the GL window")
SET(GT_WINDOW_FULLSCREEN OFF CACHE BOOL "Whether to use a fullscreen window")
SET(GT_DRAW_DIST_MULTIPLIER "1.0f" CACHE STRING "Draw distances will be multiplied by this factor")


FIND_PACKAGE(Libnxcommon)
FIND_PACKAGE(Libgtaformats)
FIND_PACKAGE(Libgta)

FIND_PATH(SQUISH_INCLUDE_PATH squish.h)
FIND_LIBRARY(SQUISH_LIB_PATH NAMES squish libsquish)

FIND_PATH(PVRTEXLIB_INCLUDE_PATH PVRTexLib.h)
FIND_LIBRARY(PVRTEXLIB_LIB_PATH NAMES PVRTexLib libPVRTexLib)

FIND_PATH(CEGUI_INCLUDE_PATH CEGUI/CEGUI.h)
FIND_LIBRARY(CEGUI_BASE_LIB_PATH NAMES CEGUIBase libCEGUIBase)

FIND_PACKAGE(Bullet COMPONENTS BulletCollision BulletDynamics LinearMath)
FIND_PACKAGE(SDL)

SET(GTAFORMATS_CONFIG_H_FOUND "")
SET(GTA_CONFIG_H_FOUND "")

FOREACH(_LIB_PATH ${LIBGTAFORMATS_INCLUDE_DIRS})
	IF(EXISTS "${_LIB_PATH}/gtaformats/config.h")
		# This is a hack which prevents automatic variable expansion in a MATCHES check in CHECK_SYMBOL_EXISTS.
		# See the comment at http://public.kitware.com/Bug/view.php?id=8226
		SET(GTAFORMATSENABLESQUISH "GTAFORMATSENABLESQUISH")
		SET(GTAFORMATSENABLEPVRTEXLIB "GTAFORMATSENABLEPVRTEXLIB")
		
		CHECK_SYMBOL_EXISTS(GTAFORMATS_ENABLE_SQUISH "${_LIB_PATH}/gtaformats/config.h" GTAFORMATSENABLESQUISH)
		CHECK_SYMBOL_EXISTS(GTAFORMATS_ENABLE_PVRTEXLIB "${_LIB_PATH}/gtaformats/config.h" GTAFORMATSENABLEPVRTEXLIB)
		SET(GTAFORMATS_ENABLE_SQUISH ${GTAFORMATSENABLESQUISH})
		SET(GTAFORMATS_ENABLE_PVRTEXLIB ${GTAFORMATSENABLEPVRTEXLIB})
		SET(GTAFORMATS_CONFIG_H_FOUND "1")
		BREAK()
	ENDIF(EXISTS "${_LIB_PATH}/gtaformats/config.h")
ENDFOREACH(_LIB_PATH)
FOREACH(_LIB_PATH ${LIBGTA_INCLUDE_DIRS})
    SET(CMAKE_REQUIRED_INCLUDES ${GTAFORMATS_INCLUDE_PATH})
	IF(EXISTS "${_LIB_PATH}/gta/config.h")
		# See above
		SET(GTAUSEOPENGLES "GTAUSEOPENGLES")
		CHECK_SYMBOL_EXISTS(GTA_USE_OPENGL_ES "${_LIB_PATH}/gta/config.h" GTAUSEOPENGLES)
		SET(GTA_USE_OPENGL_ES ${GTAUSEOPENGLES})
		SET(GTA_CONFIG_H_FOUND "1")
		BREAK()
	ENDIF(EXISTS "${_LIB_PATH}/gta/config.h")
ENDFOREACH(_LIB_PATH)

IF(GTA_USE_OPENGL_ES)
    FIND_LIBRARY(GLESV2_LIB_PATH NAMES GLESv2 libGLESv2)
    FIND_LIBRARY(EGL_LIB_PATH NAMES EGL libEGL)
    FIND_PATH(GLESV2_INCLUDE_PATH GLES2/gl2.h)
    FIND_PATH(EGL_INCLUDE_PATH EGL/egl.h)
ELSE(GTA_USE_OPENGL_ES)
    FIND_PACKAGE(OpenGL)
    FIND_LIBRARY(GLEW_LIB_PATH NAMES GLEW libGLEW glew glew32 libglew libglew32)
    FIND_PATH(GLEW_INCLUDE_PATH GL/glew.h)
ENDIF(GTA_USE_OPENGL_ES)

IF(NOT GTAFORMATS_CONFIG_H_FOUND)
	MESSAGE(SEND_ERROR "config.h of libgtaformats was not found!")
ENDIF(NOT GTAFORMATS_CONFIG_H_FOUND)
IF(NOT GTA_CONFIG_H_FOUND)
	MESSAGE(SEND_ERROR "config.h of libgta was not found!")
ENDIF(NOT GTA_CONFIG_H_FOUND)

SET(LIBRARIES "")
SET(INCLUDES "")

SET(LIBRARIES ${LIBRARIES} "${LIBGTA_LIBRARIES}")
SET(INCLUDES ${INCLUDES} ${LIBGTA_INCLUDE_DIRS})

SET(LIBRARIES ${LIBRARIES} "${LIBGTAFORMATS_LIBRARIES}")
SET(INCLUDES ${INCLUDES} ${LIBGTAFORMATS_INCLUDE_DIRS})

SET(LIBRARIES ${LIBRARIES} "${LIBNXCOMMON_LIBRARIES}")
SET(INCLUDES ${INCLUDES} ${LIBNXCOMMON_INCLUDE_DIRS})

IF(GTAFORMATS_ENABLE_SQUISH)
    MESSAGE(STATUS "libsquish support enabled.")
    SET(LIBRARIES ${LIBRARIES} "${SQUISH_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${SQUISH_INCLUDE_PATH}")
ELSE(GTAFORMATS_ENABLE_SQUISH)
    MESSAGE(STATUS "libsquish support NOT enabled.")
ENDIF(GTAFORMATS_ENABLE_SQUISH)

IF(GTAFORMATS_ENABLE_PVRTEXLIB)
    MESSAGE(STATUS "PVRTexLib support enabled.")
    SET(LIBRARIES ${LIBRARIES} "${PVRTEXLIB_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${PVRTEXLIB_INCLUDE_PATH}")
ELSE(GTAFORMATS_ENABLE_PVRTEXLIB)
    MESSAGE(STATUS "PVRTexLib support NOT enabled.")
ENDIF(GTAFORMATS_ENABLE_PVRTEXLIB)

IF(GTA_USE_OPENGL_ES)
    MESSAGE(STATUS "Using OpenGL ES 2.0.")
    SET(LIBRARIES ${LIBRARIES} "${GLESV2_LIB_PATH}" "${EGL_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${GLESV2_INCLUDE_PATH}" "${EGL_INCLUDE_PATH}")
ELSE(GTA_USE_OPENGL_ES)
    MESSAGE(STATUS "Using Desktop OpenGL")
    SET(LIBRARIES ${LIBRARIES} "${OPENGL_gl_LIBRARY}" "${GLEW_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${OPENGL_INCLUDE_DIR}" "${GLEW_INCLUDE_PATH}")
ENDIF(GTA_USE_OPENGL_ES)

SET(LIBRARIES ${LIBRARIES} ${CEGUI_BASE_LIB_PATH})
#SET(LIBRARIES ${LIBRARIES} ${CEGUI_BASE_LIB_PATH} ${CEGUI_OPENGL_RENDERER_LIB_PATH})
SET(INCLUDES ${INCLUDES} ${CEGUI_INCLUDE_PATH})

SET(LIBRARIES ${LIBRARIES} ${BULLET_LIBRARIES})
SET(INCLUDES ${INCLUDES} ${BULLET_INCLUDE_DIRS})

SET(LIBRARIES ${LIBRARIES} ${SDL_LIBRARY})
SET(INCLUDES ${INCLUDES} ${SDL_INCLUDE_DIR})

SET(INCLUDES ${INCLUDES} "${CMAKE_BINARY_DIR}/config")

SET(ADDITIONAL_INSTALL_FILES "" CACHE STRING "Paths to files which will be additionally installed to the bin subdirectory.")
SET(ADDITIONAL_INSTALL_FILES_ENABLED OFF CACHE BOOL "Set true to enable ADDITIONAL_INSTALL_FILES")

SET(SRCS main Controller)

FILE(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/config/gtatools-test")
CONFIGURE_FILE(config.cmake.h "${CMAKE_BINARY_DIR}/config/gtatools-test/config.h")

CREATE_RESOURCE(shaders/btgldebugdraw_vertex.glsl btgldebugdraw_vertex_shader)
CREATE_RESOURCE(shaders/btgldebugdraw_fragment.glsl btgldebugdraw_fragment_shader)

INCLUDE_DIRECTORIES(${INCLUDES})

ADD_EXECUTABLE (
    gtatools-test
    ${SRCS} ${MOC_COMPILED_SRCS} ${UIC_COMPILED_SRCS}
)

TARGET_LINK_LIBRARIES(gtatools-test ${LIBRARIES})

INSTALL(TARGETS gtatools-test DESTINATION bin)

IF(ADDITIONAL_INSTALL_FILES_ENABLED AND ADDITIONAL_INSTALL_FILES)
    INSTALL(FILES ${ADDITIONAL_INSTALL_FILES} DESTINATION bin)
ENDIF(ADDITIONAL_INSTALL_FILES_ENABLED AND ADDITIONAL_INSTALL_FILES)