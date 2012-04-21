# Copyright 2010-2012 David "Alemarius Nexus" Lerch
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
PROJECT(prj-gta C CXX)

INCLUDE(CheckSymbolExists)
INCLUDE(../../ResourceCompiler.cmake)

ADD_SUBDIRECTORY(gta)

SET(GTA_USE_OPENGL_ES OFF CACHE BOOL "Whether to use OpenGL ES 2.0 instead of Desktop OpenGL")
SET(GTA_VISIBILITY_PVS ON CACHE BOOL "Whether to use PVS for visibility check.")

IF(prj-gtaformats_SOURCE_DIR)
    SET(GTAFORMATS_LIB_PATH gtaformats)
    SET(GTAFORMATS_INCLUDE_PATH "${prj-gtaformats_SOURCE_DIR}" "${prj-gtaformats_BINARY_DIR}" "${prj-gtaformats_SOURCE_DIR}/gtaformats")
ELSE(prj-gtaformats_SOURCE_DIR)
    FIND_LIBRARY(GTAFORMATS_LIB_PATH NAMES gtaformats libgtaformats)
    FIND_PATH(GTAFORMATS_INCLUDE_PATH gtaformats/config.h)
ENDIF(prj-gtaformats_SOURCE_DIR)

FIND_PATH(SQUISH_INCLUDE_PATH squish.h)
FIND_LIBRARY(SQUISH_LIB_PATH NAMES squish libsquish)

FIND_PATH(PVRTEXLIB_INCLUDE_PATH PVRTexLib.h)
FIND_LIBRARY(PVRTEXLIB_LIB_PATH NAMES PVRTexLib libPVRTexLib)

FIND_PATH(CEGUI_INCLUDE_PATH CEGUI/CEGUI.h)
FIND_LIBRARY(CEGUI_BASE_LIB_PATH NAMES CEGUIBase libCEGUIBase)

FIND_PACKAGE(Boost)
FIND_PACKAGE(Bullet COMPONENTS BulletCollision BulletDynamics LinearMath)

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

IF(APPLE)
    FIND_PATH(ICONV_INCLUDE_PATH iconv.h)
    FIND_LIBRARY(ICONV_LIB_PATH NAMES iconv libiconv)
ENDIF(APPLE)

SET(GTAFORMATS_CONFIG_H_FOUND "")

FOREACH(_LIB_PATH ${GTAFORMATS_INCLUDE_PATH})
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

IF(NOT GTAFORMATS_CONFIG_H_FOUND)
	MESSAGE(SEND_ERROR "config.h of libgtaformats was not found!")
ENDIF(NOT GTAFORMATS_CONFIG_H_FOUND)

SET(LIBRARIES "")
SET(INCLUDES "")

SET(LIBRARIES ${LIBRARIES} "${GTAFORMATS_LIB_PATH}")
SET(INCLUDES ${INCLUDES} ${GTAFORMATS_INCLUDE_PATH})

IF(GTAFORMATS_ENABLE_SQUISH)
    SET(LIBRARIES ${LIBRARIES} "${SQUISH_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${SQUISH_INCLUDE_PATH}")
ENDIF(GTAFORMATS_ENABLE_SQUISH)

IF(GTAFORMATS_ENABLE_PVRTEXLIB)
    SET(LIBRARIES ${LIBRARIES} "${PVRTEXLIB_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${PVRTEXLIB_INCLUDE_PATH}")
ENDIF(GTAFORMATS_ENABLE_PVRTEXLIB)

IF(GTA_USE_OPENGL_ES)
    SET(LIBRARIES ${LIBRARIES} "${GLESV2_LIB_PATH}" "${EGL_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${GLESV2_INCLUDE_PATH}" "${EGL_INCLUDE_PATH}")
ELSE(GTA_USE_OPENGL_ES)
    SET(LIBRARIES ${LIBRARIES} "${OPENGL_gl_LIBRARY}" "${GLEW_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${OPENGL_INCLUDE_DIR}" "${GLEW_INCLUDE_PATH}")
ENDIF(GTA_USE_OPENGL_ES)

SET(INCLUDES ${INCLUDES} "${Boost_INCLUDE_DIRS}")

SET(LIBRARIES ${LIBRARIES} ${BULLET_LIBRARIES})
SET(INCLUDES ${INCLUDES} ${BULLET_INCLUDE_DIRS})

SET(LIBRARIES ${LIBRARIES} "${CEGUI_BASE_LIB_PATH}")
SET(INCLUDES ${INCLUDES} "${CEGUI_INCLUDE_PATH}")

#SET(LIBRARIES ${LIBRARIES} "${OPENGL_gl_LIBRARY}")
#SET(INCLUDES ${INCLUDES} "${OPENGL_INCLUDE_DIR}")

IF(APPLE)
    SET(LIBRARIES ${LIBRARIES} "${ICONV_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${ICONV_INCLUDE_PATH}")
ENDIF(APPLE)

SET(INCLUDES ${INCLUDES} "${CMAKE_CURRENT_BINARY_DIR}")

INCLUDE_DIRECTORIES(${INCLUDES})

CREATE_RESOURCE(gta/shaders/global.glsl global_shader)
CREATE_RESOURCE(gta/shaders/global_vertex.glsl global_vertex_shader)
CREATE_RESOURCE(gta/shaders/global_fragment.glsl global_fragment_shader)

CREATE_RESOURCE(gta/shaders/vertex_default.glsl vertex_default_shader)
CREATE_RESOURCE(gta/shaders/fragment_default.glsl fragment_default_shader)

CREATE_RESOURCE(gta/shaders/static/shade_vertex.glsl shade_vertex_shader)
CREATE_RESOURCE(gta/shaders/static/shade_fragment.glsl shade_fragment_shader)

CREATE_RESOURCE(gta/shaders/depth_peeling/peel_layer_vertex.glsl dp_peel_layer_vertex_shader)
CREATE_RESOURCE(gta/shaders/depth_peeling/peel_layer_fragment.glsl dp_peel_layer_fragment_shader)

CREATE_RESOURCE(gta/shaders/depth_peeling/blend_layer_vertex.glsl dp_blend_layer_vertex_shader)
CREATE_RESOURCE(gta/shaders/depth_peeling/blend_layer_fragment.glsl dp_blend_layer_fragment_shader)

CREATE_RESOURCE(gta/shaders/depth_peeling/blend_final_vertex.glsl dp_blend_final_vertex_shader)
CREATE_RESOURCE(gta/shaders/depth_peeling/blend_final_fragment.glsl dp_blend_final_fragment_shader)

CREATE_RESOURCE(gta/shaders/weighted_average/vertex.glsl wavg_vertex_shader)
CREATE_RESOURCE(gta/shaders/weighted_average/fragment.glsl wavg_fragment_shader)

CREATE_RESOURCE(gta/shaders/weighted_average/final_vertex.glsl wavg_final_vertex_shader)
CREATE_RESOURCE(gta/shaders/weighted_average/final_fragment.glsl wavg_final_fragment_shader)

CREATE_RESOURCE(gta/shaders/cegui/cegui_vertex.glsl cegui_vertex_shader)
CREATE_RESOURCE(gta/shaders/cegui/cegui_fragment.glsl cegui_fragment_shader)

CREATE_RESOURCE(gta/shaders/debug/render_depth_2d_vertex.glsl render_depth_2d_vertex_shader)
CREATE_RESOURCE(gta/shaders/debug/render_depth_2d_fragment.glsl render_depth_2d_fragment_shader)

CREATE_RESOURCE(gta/shaders/debug/render_depth_rect_vertex.glsl render_depth_rect_vertex_shader)
CREATE_RESOURCE(gta/shaders/debug/render_depth_rect_fragment.glsl render_depth_rect_fragment_shader)

CREATE_RESOURCE(gta/shaders/anim/shade_vertex.glsl anim_shade_vertex_shader)
CREATE_RESOURCE(gta/shaders/anim/shade_fragment.glsl anim_shade_fragment_shader)

CREATE_RESOURCE(gta/shaders/anim/shade_vertex.glsl anim_shade_vertex_shader)
CREATE_RESOURCE(gta/shaders/anim/shade_fragment.glsl anim_shade_fragment_shader)

ADD_LIBRARY(gta STATIC ${SRCS})
TARGET_LINK_LIBRARIES(gta ${LIBRARIES})

INSTALL(TARGETS gta DESTINATION lib)
INSTALL(	DIRECTORY gta
		DESTINATION include
		FILES_MATCHING
			PATTERN *.h
			PATTERN .svn EXCLUDE
			PATTERN *.cmake.h EXCLUDE
	)
INSTALL(    FILES "${CMAKE_CURRENT_BINARY_DIR}/gta/config.h"
        DESTINATION "include/gta"
    )

