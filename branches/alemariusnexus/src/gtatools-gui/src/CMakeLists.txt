# Copyright 2010 David "Alemarius Nexus" Lerch
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

IF(UNIX)
	SET(GL_DEFAULT_LIB GL)
	SET(GLU_DEFAULT_LIB GLU)
ELSE(UNIX) # Windows...
	SET(GL_DEFAULT_LIB opengl32)
	SET(GLU_DEFAULT_LIB glu32)
ENDIF(UNIX)

SET(GTAFORMATS_LIB_PATH "gtaformats" CACHE PATH "Path to (or name of) the libgtaformats library file")
SET(GL_INCLUDE_PATH "" CACHE PATH "Directory containing the OpenGL header file(s)")
SET(GL_LIB_PATH "${GL_DEFAULT_LIB}" CACHE PATH "Path to (or name of) the OpenGL library file")
SET(GLU_INCLUDE_PATH "" CACHE PATH "Directory containing the OpenGL Utility (GLU) header file(s)")
SET(GLU_LIB_PATH "${GLU_DEFAULT_LIB}" CACHE PATH "Path to (or name of) the OpenGL Utility (GLU) library file")

FIND_PACKAGE(Qt4 4.6.0 COMPONENTS QtCore QtGui QtXml REQUIRED)
INCLUDE(${QT_USE_FILE})

IF(NOT GTAFORMATS_LIB_PATH)
	SET(GTAFORMATS_LIB_PATH gtaformats)
ENDIF(NOT GTAFORMATS_LIB_PATH)

QT4_WRAP_CPP(MOC_COMPILED_SRCS ${MOC_SRCS})
QT4_WRAP_UI(UIC_COMPILED_SRCS ${UIC_SRCS})
QT4_ADD_TRANSLATION(QM_FILES gtatools-gui_en.ts gtatools-gui_de.ts)

INCLUDE_DIRECTORIES(${GL_INCLUDE_PATH} ${GLU_INCLUDE_PATH} ${GTAFORMATS_INCLUDE_PATH} ${CMAKE_BINARY_DIR}/gtatools-gui/build/src
         ${CMAKE_BINARY_DIR} ${CMAKE_BINARY_DIR}/gtatools-gui/build ${INCS})

ADD_EXECUTABLE(gtatools-gui ${SRCS} ${MOC_COMPILED_SRCS} ${UIC_COMPILED_SRCS} ${QM_FILES})
TARGET_LINK_LIBRARIES(gtatools-gui ${GTAFORMATS_LIB_PATH} ${QT_LIBRARIES} ${GL_LIB_PATH} ${GLU_LIB_PATH})

INSTALL(TARGETS gtatools-gui DESTINATION bin)
INSTALL(FILES ${QM_FILES} DESTINATION bin)

