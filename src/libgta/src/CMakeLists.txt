# Copyright 2010-2014 David "Alemarius Nexus" Lerch
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


CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(prj-gta C CXX)

SET(GTATOOLS_INSTALL_ENABLED OFF CACHE BOOL "If true, CMake INSTALL commands will be run for gtatools.")

IF(NOT TARGET nxcommon)
    FIND_PACKAGE(Nxcommon REQUIRED CONFIG)
ENDIF()

INCLUDE(CheckSymbolExists)
INCLUDE(NxResourceCompiler)

IF(NOT NXCOMMON_BULLET_SUPPORT)
    MESSAGE(SEND_ERROR "libnxcommon was built without Bullet support. You need to build it with Bullet support enabled!")
ENDIF()
IF(NOT NXCOMMON_OPENGL_ENABLED)
    MESSAGE(SEND_ERROR "libnxcommon was built without OpenGL support. You need to build it with OpenGL support enabled!")
ENDIF()

IF(NOT TARGET gtaformats)
    FIND_PACKAGE(Libgtaformats REQUIRED CONFIG)
ENDIF()

ADD_SUBDIRECTORY(gta)

SET(GTA_USE_OPENGL_ES OFF CACHE BOOL "Whether to use OpenGL ES 2.0 instead of Desktop OpenGL")
SET(GTA_VISIBILITY_PVS ON CACHE BOOL "Whether to use PVS for visibility check.")

FIND_PACKAGE(Nxcommon REQUIRED)
FIND_PACKAGE(Libgtaformats REQUIRED)

IF(GTAFORMATS_ENABLE_SQUISH)
    FIND_PACKAGE(Libsquish REQUIRED)
ENDIF(GTAFORMATS_ENABLE_SQUISH)

FIND_PATH(LUA_INCLUDE_DIRS luajit.h PATH_SUFFIXES luajit luajit-2.1)
FIND_LIBRARY(LUA_LIBRARIES NAMES luajit libluajit luajit-5.1 libluajit-5.1)

IF(GTAFORMATS_ENABLE_PVRTEXLIB)
    FIND_PACKAGE(PVRTexLib REQUIRED)
ENDIF(GTAFORMATS_ENABLE_PVRTEXLIB)

FIND_PACKAGE(CEGUI REQUIRED)

FIND_PACKAGE(Bullet REQUIRED COMPONENTS BulletCollision BulletDynamics LinearMath)

IF(NXCOMMON_QT_SUPPORT STREQUAL "qt4")
    FIND_PACKAGE(Qt4 4.6.0 COMPONENTS QtCore REQUIRED)
    INCLUDE(${QT_USE_FILE})
ELSEIF(NXCOMMON_QT_SUPPORT STREQUAL "qt5")
    FIND_PACKAGE(Qt5Core)
ENDIF()

IF(GTA_USE_OPENGL_ES)
    FIND_LIBRARY(GLESV2_LIBRARIES NAMES GLESv2 libGLESv2)
    FIND_LIBRARY(EGL_LIBRARIES NAMES EGL libEGL)
    FIND_PATH(GLESV2_INCLUDE_DIRS GLES2/gl2.h)
    FIND_PATH(EGL_INCLUDE_DIRS EGL/egl.h)
ELSE(GTA_USE_OPENGL_ES)
    SET(OpenGL_GL_PREFERENCE LEGACY)
    FIND_PACKAGE(OpenGL)
    FIND_LIBRARY(GLEW_LIBRARIES NAMES GLEW libGLEW glew glew32 libglew libglew32)
    FIND_PATH(GLEW_INCLUDE_DIRS GL/glew.h)
ENDIF(GTA_USE_OPENGL_ES)

IF(APPLE)
    FIND_PATH(ICONV_INCLUDE_PATH iconv.h)
    FIND_LIBRARY(ICONV_LIB_PATH NAMES iconv libiconv)
ENDIF(APPLE)

SET(GTAFORMATS_CONFIG_H_FOUND "")

#FOREACH(_LIB_PATH ${LIBGTAFORMATS_INCLUDE_DIRS})
#	IF(EXISTS "${_LIB_PATH}/gtaformats/config.h")
#		# This is a hack which prevents automatic variable expansion in a MATCHES check in CHECK_SYMBOL_EXISTS.
#		# See the comment at http://public.kitware.com/Bug/view.php?id=8226
#		SET(GTAFORMATSENABLESQUISH "GTAFORMATSENABLESQUISH")
#		SET(GTAFORMATSENABLEPVRTEXLIB "GTAFORMATSENABLEPVRTEXLIB")
#		
#		CHECK_SYMBOL_EXISTS(GTAFORMATS_ENABLE_SQUISH "${_LIB_PATH}/gtaformats/config.h" GTAFORMATSENABLESQUISH)
#		CHECK_SYMBOL_EXISTS(GTAFORMATS_ENABLE_PVRTEXLIB "${_LIB_PATH}/gtaformats/config.h" GTAFORMATSENABLEPVRTEXLIB)
#		SET(GTAFORMATS_ENABLE_SQUISH ${GTAFORMATSENABLESQUISH})
#		SET(GTAFORMATS_ENABLE_PVRTEXLIB ${GTAFORMATSENABLEPVRTEXLIB})
#		SET(GTAFORMATS_CONFIG_H_FOUND "1")
#		BREAK()
#	ENDIF(EXISTS "${_LIB_PATH}/gtaformats/config.h")
#ENDFOREACH(_LIB_PATH)
#
#IF(NOT GTAFORMATS_CONFIG_H_FOUND)
#    MESSAGE(SEND_ERROR "config.h of libgtaformats was not found!")
#ENDIF(NOT GTAFORMATS_CONFIG_H_FOUND)

SET(LIBRARIES "")
SET(INCLUDES "")

#SET(LIBRARIES ${LIBRARIES} "${LIBNXCOMMON_LIBRARIES}")
#SET(INCLUDES ${INCLUDES} ${LIBNXCOMMON_INCLUDE_DIRS})

#SET(LIBRARIES ${LIBRARIES} "${LIBGTAFORMATS_LIBRARIES}")
#SET(INCLUDES ${INCLUDES} ${LIBGTAFORMATS_INCLUDE_DIRS})

IF(GTAFORMATS_ENABLE_SQUISH)
    SET(LIBRARIES ${LIBRARIES} "${LIBSQUISH_LIBRARIES}")
    SET(INCLUDES ${INCLUDES} "${LIBSQUISH_INCLUDE_DIRS}")
ENDIF(GTAFORMATS_ENABLE_SQUISH)

SET(LIBRARIES ${LIBRARIES} "${LUA_LIBRARIES}")
SET(INCLUDES ${INCLUDES} "${LUA_INCLUDE_DIRS}")

IF(GTAFORMATS_ENABLE_PVRTEXLIB)
    SET(LIBRARIES ${LIBRARIES} "${PVRTEXLIB_LIBRARIES}")
    SET(INCLUDES ${INCLUDES} "${PVRTEXLIB_INCLUDE_DIRS}")
ENDIF(GTAFORMATS_ENABLE_PVRTEXLIB)

IF(GTA_USE_OPENGL_ES)
    SET(LIBRARIES ${LIBRARIES} "${GLESV2_LIBRARIES}" "${EGL_LIBRARIES}")
    SET(INCLUDES ${INCLUDES} "${GLESV2_INCLUDE_DIRS}" "${EGL_INCLUDE_DIRS}")
ELSE(GTA_USE_OPENGL_ES)
    SET(LIBRARIES ${LIBRARIES} "${OPENGL_gl_LIBRARY}" "${GLEW_LIBRARIES}")
    SET(INCLUDES ${INCLUDES} "${OPENGL_INCLUDE_DIR}" "${GLEW_INCLUDE_DIRS}")
ENDIF(GTA_USE_OPENGL_ES)

SET(LIBRARIES ${LIBRARIES} ${BULLET_LIBRARIES})
SET(INCLUDES ${INCLUDES} ${BULLET_INCLUDE_DIRS})

SET(LIBRARIES ${LIBRARIES} "${CEGUI_LIBRARIES}")
SET(INCLUDES ${INCLUDES} "${CEGUI_INCLUDE_DIRS}")

IF(APPLE)
    SET(LIBRARIES ${LIBRARIES} "${ICONV_LIB_PATH}")
    SET(INCLUDES ${INCLUDES} "${ICONV_INCLUDE_PATH}")
ENDIF(APPLE)

IF(UNIX)
    SET(LIBRARIES ${LIBRARIES} "pthread")
    SET(LIBRARIES ${LIBRARIES} "dl")
ENDIF(UNIX)

#SET(INCLUDES ${INCLUDES} "${CMAKE_CURRENT_BINARY_DIR}")

#INCLUDE_DIRECTORIES(${INCLUDES})

ADD_CUSTOM_TARGET(libgta_res)

CREATE_RESOURCE(libgta_res gta/shaders/test_vertex.glsl test_vertex_shader)
CREATE_RESOURCE(libgta_res gta/shaders/test_vertex2.glsl test_vertex2_shader)

CREATE_RESOURCE(libgta_res gta/shaders/test_fragment.glsl test_fragment_shader)
CREATE_RESOURCE(libgta_res gta/shaders/test_fragment2.glsl test_fragment2_shader)

CREATE_RESOURCE(libgta_res gta/shaders/dp_test_vertex.glsl dp_test_vertex_shader)
CREATE_RESOURCE(libgta_res gta/shaders/dp_test_fragment.glsl dp_test_fragment_shader)

CREATE_RESOURCE(libgta_res gta/shaders/global.glsl global_shader)
CREATE_RESOURCE(libgta_res gta/shaders/global_vertex.glsl global_vertex_shader)
CREATE_RESOURCE(libgta_res gta/shaders/global_fragment.glsl global_fragment_shader)

CREATE_RESOURCE(libgta_res gta/shaders/vertex_default.glsl vertex_default_shader)
CREATE_RESOURCE(libgta_res gta/shaders/fragment_default.glsl fragment_default_shader)

CREATE_RESOURCE(libgta_res gta/shaders/static/shade_vertex.glsl shade_vertex_shader)
CREATE_RESOURCE(libgta_res gta/shaders/static/shade_fragment.glsl shade_fragment_shader)

CREATE_RESOURCE(libgta_res gta/shaders/depth_peeling/peel_layer_vertex.glsl dp_peel_layer_vertex_shader)
CREATE_RESOURCE(libgta_res gta/shaders/depth_peeling/peel_layer_fragment.glsl dp_peel_layer_fragment_shader)

CREATE_RESOURCE(libgta_res gta/shaders/depth_peeling/blend_layer_vertex.glsl dp_blend_layer_vertex_shader)
CREATE_RESOURCE(libgta_res gta/shaders/depth_peeling/blend_layer_fragment.glsl dp_blend_layer_fragment_shader)

CREATE_RESOURCE(libgta_res gta/shaders/depth_peeling/blend_final_vertex.glsl dp_blend_final_vertex_shader)
CREATE_RESOURCE(libgta_res gta/shaders/depth_peeling/blend_final_fragment.glsl dp_blend_final_fragment_shader)

CREATE_RESOURCE(libgta_res gta/shaders/weighted_average/vertex.glsl wavg_vertex_shader)
CREATE_RESOURCE(libgta_res gta/shaders/weighted_average/fragment.glsl wavg_fragment_shader)

CREATE_RESOURCE(libgta_res gta/shaders/weighted_average/final_vertex.glsl wavg_final_vertex_shader)
CREATE_RESOURCE(libgta_res gta/shaders/weighted_average/final_fragment.glsl wavg_final_fragment_shader)

CREATE_RESOURCE(libgta_res gta/shaders/cegui/cegui_vertex.glsl cegui_vertex_shader)
CREATE_RESOURCE(libgta_res gta/shaders/cegui/cegui_fragment.glsl cegui_fragment_shader)

CREATE_RESOURCE(libgta_res gta/shaders/debug/render_depth_2d_vertex.glsl render_depth_2d_vertex_shader)
CREATE_RESOURCE(libgta_res gta/shaders/debug/render_depth_2d_fragment.glsl render_depth_2d_fragment_shader)

CREATE_RESOURCE(libgta_res gta/shaders/debug/render_depth_rect_vertex.glsl render_depth_rect_vertex_shader)
CREATE_RESOURCE(libgta_res gta/shaders/debug/render_depth_rect_fragment.glsl render_depth_rect_fragment_shader)

CREATE_RESOURCE(libgta_res gta/shaders/anim/shade_vertex.glsl anim_shade_vertex_shader)
CREATE_RESOURCE(libgta_res gta/shaders/anim/shade_fragment.glsl anim_shade_fragment_shader)

CREATE_RESOURCE(libgta_res gta/shaders/lighting_vertex.glsl lighting_vertex_shader)

ADD_LIBRARY(gta STATIC ${SRCS})
ADD_DEPENDENCIES(gta libgta_res)
TARGET_LINK_LIBRARIES(gta PUBLIC ${LIBRARIES} nxcommon gtaformats)
TARGET_INCLUDE_DIRECTORIES(gta PUBLIC ${INCLUDES} $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

IF(GTATOOLS_INSTALL_ENABLED)
    TARGET_ADD_INSTALL_STATE_DEFINE(gta GTA)

    INSTALL(TARGETS gta EXPORT LibgtaTargets DESTINATION lib)
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

    EXPORT (
        EXPORT LibgtaTargets
        FILE "${CMAKE_CURRENT_BINARY_DIR}/cmakecfg/LibgtaTargets.cmake"
    )

    CONFIGURE_FILE(LibgtaConfig.cmake.in "${CMAKE_CURRENT_BINARY_DIR}/cmakecfg/LibgtaConfig.cmake" @ONLY)

    INSTALL (
        EXPORT LibgtaTargets
        FILE LibgtaTargets.cmake
        DESTINATION lib/cmake
    )

    INSTALL (
        FILES "${CMAKE_CURRENT_BINARY_DIR}/cmakecfg/LibgtaConfig.cmake"
        DESTINATION lib/cmake
    )
ENDIF()

