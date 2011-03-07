# Copyright 2010-2011 David "Alemarius Nexus" Lerch
# 
# This file is part of gtatools-gui.
#
# gtatools-gui is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# gtatools-gui is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(prj-gtatools-gui CXX)

ADD_SUBDIRECTORY(src)

IF(prj-gtaformats_SOURCE_DIR)
    SET(GTAFORMATS_INCLUDE_PATH "${prj-gtaformats_SOURCE_DIR}" CACHE PATH "Path to the gtaformats include dir (EXCLUDING the 'gtaformats' part)")
ELSE(prj-gtaformats_SOURCE_DIR)
    SET(GTAFORMATS_INCLUDE_PATH "" CACHE PATH "Path to the gtaformats include dir (EXCLUDING the 'gtaformats' part)")
ENDIF(prj-gtaformats_SOURCE_DIR)
IF(prj-gta_SOURCE_DIR)
    SET(GTA_INCLUDE_PATH "${prj-gta_SOURCE_DIR}" CACHE PATH "Path to the libgta include dir (ECLUDING the 'gta' part)")
ELSE(prj-gta_SOURCE_DIR)
    SET(GTA_INCLUDE_PATH "" CACHE PATH "Path to the libgta include dir (ECLUDING the 'gta' part)")
ENDIF(prj-gta_SOURCE_DIR)

IF(WIN32)
    SET(GL_DEFAULT_LIB opengl32)
	SET(GLU_DEFAULT_LIB glu32)
	SET(GLEW_DEFAULT_LIB glew32)
ELSE(WIN32)
    SET(GL_DEFAULT_LIB GL)
	SET(GLU_DEFAULT_LIB GLU)
	SET(GLEW_DEFAULT_LIB GLEW)
ENDIF(WIN32)

SET(GTAFORMATS_LIB_PATH "gtaformats" CACHE FILEPATH "Path to (or name of) the libgtaformats library file")
SET(GTA_LIB_PATH "gta" CACHE FILEPATH "Path to (or name of) the libgta library file")
SET(GL_INCLUDE_PATH "" CACHE PATH "Directory containing the OpenGL header file(s)")
SET(GL_LIB_PATH "${GL_DEFAULT_LIB}" CACHE FILEPATH "Path to (or name of) the OpenGL library file")
SET(GLU_INCLUDE_PATH "" CACHE PATH "Directory containing the OpenGL Utility (GLU) header file(s)")
SET(GLU_LIB_PATH "${GLU_DEFAULT_LIB}" CACHE FILEPATH "Path to (or name of) the OpenGL Utility (GLU) library file")
SET(GLEW_INCLUDE_PATH "" CACHE PATH "Directory containing the OpenGL Extension Wrangler (GLEW) header files")
SET(GLEW_LIB_PATH "${GLEW_DEFAULT_LIB}" CACHE FILEPATH "Path to (or name of) the OpenGL Extension Wrangler (GLEW) library file")
SET(SQUISH_INCLUDE_PATH "" CACHE PATH "Directory containing libsquish header files.")
SET(SQUISH_LIB_PATH "squish" CACHE FILEPATH "Path to (or name of) libsquish library file")
SET(BULLET_DYNAMICS_LIB_PATH "BulletDynamics" CACHE FILEPATH "Path to (or name of) the BulletDynamics library file")
SET(BULLET_COLLISION_LIB_PATH "BulletCollision" CACHE FILEPATH "Path to (or name of) the BulletCollision library file")
SET(BULLET_LINEAR_MATH_LIB_PATH "LinearMath" CACHE FILEPATH "Path to (or name of) the Bullet LinearMath library file")
SET(BULLET_INCLUDE_PATH "" CACHE PATH "Directory containing Bullet header files")

FIND_PACKAGE(Qt4 4.6.0 COMPONENTS QtCore QtGui QtXml QtOpenGL qtmain REQUIRED)

IF(WIN32)
    STRING(REGEX MATCH "-lGL" QTOPENGL_DEPS_UNIX "${QT_QTOPENGL_LIB_DEPENDENCIES}")
    
    IF(QTOPENGL_DEPS_UNIX)
        SET(QT_QTOPENGL_LIB_DEPENDENCIES "")
    ENDIF(QTOPENGL_DEPS_UNIX)
ENDIF(WIN32)

IF(UNIX)
    SET(THREAD_LIB "pthread")
ELSE(UNIX)
    SET(THREAD_LIB "")
ENDIF(UNIX)

INCLUDE(${QT_USE_FILE})

IF(NOT GTAFORMATS_LIB_PATH)
	SET(GTAFORMATS_LIB_PATH gtaformats)
ENDIF(NOT GTAFORMATS_LIB_PATH)

SET_SOURCE_FILES_PROPERTIES(gtatools-gui_en.ts gtatools-gui_de.ts PROPERTIES
    OUTPUT_LOCATION "${CMAKE_SOURCE_DIR}/gtatools-gui/src")

QT4_WRAP_CPP(MOC_COMPILED_SRCS ${MOC_SRCS})
QT4_WRAP_UI(UIC_COMPILED_SRCS ${UIC_SRCS})
QT4_ADD_TRANSLATION(QM_FILES gtatools-gui_en.ts gtatools-gui_de.ts)

QT4_ADD_RESOURCES(QT_RESOURCE_FILES "${CMAKE_SOURCE_DIR}/gtatools-gui/src/resources.qrc")

SET(EXECUTABLE_PROPERTIES "")
IF(WIN32)
    SET(EXECUTABLE_PROPERTIES "WIN32")
ENDIF(WIN32)

INCLUDE_DIRECTORIES(${GL_INCLUDE_PATH} ${GLU_INCLUDE_PATH} ${GTAFORMATS_INCLUDE_PATH} ${GTA_INCLUDE_PATH}
        ${CMAKE_BINARY_DIR}/gtatools-gui/build/src ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/gtatools-gui/build
        ${INCS} ${GLEW_INCLUDE_PATH} ${SQUISH_INCLUDE_PATH} ${BULLET_INCLUDE_PATH})

ADD_EXECUTABLE(gtatools-gui ${EXECUTABLE_PROPERTIES} ${SRCS} ${MOC_COMPILED_SRCS} ${UIC_COMPILED_SRCS} ${QM_FILES}
        ${QT_RESOURCE_FILES})
TARGET_LINK_LIBRARIES(gtatools-gui ${GTA_LIB_PATH} ${GTAFORMATS_LIB_PATH} ${QT_LIBRARIES} ${GL_LIB_PATH}
        ${GLU_LIB_PATH} ${GLEW_LIB_PATH} ${THREAD_LIB} ${SQUISH_LIB_PATH} ${BULLET_DYNAMICS_LIB_PATH}
        ${BULLET_COLLISION_LIB_PATH} ${BULLET_LINEAR_MATH_LIB_PATH})

INSTALL(TARGETS gtatools-gui DESTINATION bin)
INSTALL(FILES ${QM_FILES} DESTINATION bin)

