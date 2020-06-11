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

CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
PROJECT(gtatools-test C CXX)

CMAKE_POLICY(SET CMP0020 NEW)

INCLUDE(CheckSymbolExists)



IF(NOT TARGET nxcommon)
    FIND_PACKAGE(Libnxcommon REQUIRED CONFIG)
ENDIF()

INCLUDE(NxResourceCompiler)

IF(NOT TARGET gtaformats)
    FIND_PACKAGE(Libgtaformats REQUIRED CONFIG)
ENDIF()

IF(NOT TARGET gta)
    FIND_PACKAGE(Libgta REQUIRED CONFIG)
ENDIF()

IF(GTAFORMATS_ENABLE_SQUISH)
    FIND_PACKAGE(Libsquish REQUIRED)
ENDIF()

IF(GTAFORMATS_ENABLE_PVRTEXLIB)
    FIND_PACKAGE(PVRTexLib REQUIRED)
ENDIF()

FIND_PACKAGE(CEGUI REQUIRED)

FIND_PACKAGE(Bullet REQUIRED COMPONENTS BulletCollision BulletDynamics LinearMath)
FIND_PACKAGE(SDL REQUIRED)

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

SET(LIBRARIES gta gtaformats nxcommon)
SET(INCLUDES "")

IF(GTAFORMATS_ENABLE_SQUISH)
    SET(LIBRARIES ${LIBRARIES} "${LIBSQUISH_LIBRARIES}")
    SET(INCLUDES ${INCLUDES} "${LIBSQUISH_INCLUDE_DIRS}")
ENDIF()

IF(GTAFORMATS_ENABLE_PVRTEXLIB)
    SET(LIBRARIES ${LIBRARIES} "${PVRTEXLIB_LIBRARIES}")
    SET(INCLUDES ${INCLUDES} "${PVRTEXLIB_INCLUDE_DIRS}")
ENDIF()

IF(GTA_USE_OPENGL_ES)
    SET(LIBRARIES ${LIBRARIES} "${GLESV2_LIBRARIES}" "${EGL_LIBRARIES}")
    SET(INCLUDES ${INCLUDES} "${GLESV2_INCLUDE_DIRS}" "${EGL_INCLUDE_DIRS}")
ELSE(GTA_USE_OPENGL_ES)
    SET(LIBRARIES ${LIBRARIES} "${OPENGL_gl_LIBRARY}" "${GLEW_LIBRARIES}")
    SET(INCLUDES ${INCLUDES} "${OPENGL_INCLUDE_DIR}" "${GLEW_INCLUDE_DIRS}")
ENDIF(GTA_USE_OPENGL_ES)

SET(LIBRARIES ${LIBRARIES} ${CEGUI_LIBRARIES})
SET(INCLUDES ${INCLUDES} ${CEGUI_INCLUDE_DIRS})

SET(LIBRARIES ${LIBRARIES} ${BULLET_LIBRARIES})
SET(INCLUDES ${INCLUDES} ${BULLET_INCLUDE_DIRS})

SET(LIBRARIES ${LIBRARIES} ${SDL_LIBRARY})
SET(INCLUDES ${INCLUDES} ${SDL_INCLUDE_DIR})

SET(INCLUDES ${INCLUDES} "${CMAKE_BINARY_DIR}/config")

SET(ADDITIONAL_INSTALL_FILES "" CACHE STRING "Paths to files which will be additionally installed to the bin subdirectory.")
SET(ADDITIONAL_INSTALL_FILES_ENABLED OFF CACHE BOOL "Set true to enable ADDITIONAL_INSTALL_FILES")

SET(SRCS main Controller VehicleController SphericalCameraController BulletGLDebugDraw)

FILE(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/config/gtatools-test")
CONFIGURE_FILE(config.cmake.h "${CMAKE_BINARY_DIR}/config/gtatools-test/config.h")

ADD_CUSTOM_TARGET(gtatools-test_res)

CREATE_RESOURCE(gtatools-test_res shaders/btgldebugdraw_vertex.glsl btgldebugdraw_vertex_shader)
CREATE_RESOURCE(gtatools-test_res shaders/btgldebugdraw_fragment.glsl btgldebugdraw_fragment_shader)

INCLUDE_DIRECTORIES(${INCLUDES})

ADD_EXECUTABLE (
    gtatools-test
    ${SRCS} ${MOC_COMPILED_SRCS} ${UIC_COMPILED_SRCS}
)
ADD_DEPENDENCIES(gtatools-test gtatools-test_res)
TARGET_COMPILE_DEFINITIONS(gtatools-test PRIVATE -DWIN32_LEAN_AND_MEAN)

TARGET_LINK_LIBRARIES(gtatools-test ${LIBRARIES})

INSTALL(TARGETS gtatools-test DESTINATION bin)

IF(ADDITIONAL_INSTALL_FILES_ENABLED AND ADDITIONAL_INSTALL_FILES)
    INSTALL(FILES ${ADDITIONAL_INSTALL_FILES} DESTINATION bin)
ENDIF(ADDITIONAL_INSTALL_FILES_ENABLED AND ADDITIONAL_INSTALL_FILES)
