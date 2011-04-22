# Copyright 2010-2011 David "Alemarius Nexus" Lerch
# 
# This file is part of gtatools.
#
# gtatools is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# gtatools is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with gtatools.  If not, see <http://www.gnu.org/licenses/>.

# Additional permissions are granted, which are listed in the file
# GPLADDITIONS.

# Additional permissions are granted, which are listed in the file
# GPLADDITIONS.



CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(gtatools CXX)

SET(GTATOOLS_VERSION_MAJOR "0") # Major version, must be numeric
SET(GTATOOLS_VERSION_MINOR "2") # Minor version, must be numeric
SET(GTATOOLS_VERSION_SUFFIX "-dev") # A suffix for the version string, could e.g. be 'b' for beta versions, or empty

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "gtatools - A collection of tools and libraries for loading formats of the GTA game series.")
SET(CPACK_PACKAGE_VENDOR "David 'Alemarius Nexus' Lerch")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
#SET(CPACK_PACKAGE_VERSION_MAJOR ${GTATOOLS_VERSION_MAJOR})
#SET(CPACK_PACKAGE_VERSION_MINOR ${GTATOOLS_VERSION_MINOR})
#SET(CPACK_PACKAGE_VERSION_PATCH ${GTATOOLS_VERSION_PATCH}${GTATOOLS_VERSION_SUFFIX})
SET(CPACK_PACKAGE_VERSION "${GTATOOLS_VERSION_MAJOR}.${GTATOOLS_VERSION_MINOR}${GTATOOLS_VERSION_SUFFIX}")
SET(CPACK_SOURCE_IGNORE_FILES "\\\\.svn;.*~")


FIND_PACKAGE(Subversion)

SET(_CMAKE_HAVE_SVNREV OFF)
IF(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/.svn")
    IF(Subversion_FOUND)
        Subversion_WC_INFO(${CMAKE_SOURCE_DIR} Project)
        SET(GTATOOLS_VERSION_SVNREV "${Project_WC_REVISION}")
        SET(_CMAKE_HAVE_SVNREV ON)
    ENDIF(Subversion_FOUND)
ENDIF(IS_DIRECTORY "${CMAKE_SOURCE_DIR}/.svn")

IF(GTATOOLS_VERSION_SVNREV)
    SET(GTATOOLS_VERSION "${GTATOOLS_VERSION_MAJOR}.${GTATOOLS_VERSION_MINOR}${GTATOOLS_VERSION_SUFFIX}-r${GTATOOLS_VERSION_SVNREV}")
ELSE(GTATOOLS_VERSION_SVNREV)
    SET(GTATOOLS_VERSION "${GTATOOLS_VERSION_MAJOR}.${GTATOOLS_VERSION_MINOR}${GTATOOLS_VERSION_SUFFIX}")
ENDIF(GTATOOLS_VERSION_SVNREV)


SET(LIBGTAFORMATS_BUILD ON CACHE BOOL "Whether you want to build libgtaformats")
SET(LIBGTA_BUILD ON CACHE BOOL "Whether you want to build libgta")
SET(GTAIMG_BUILD ON CACHE BOOL "Whether you want to build gtaimg")
SET(GTATXD_BUILD ON CACHE BOOL "Whether you want to build gtatxd")
SET(GTADFF_BUILD OFF CACHE BOOL "Whether you want to build gtadff")
SET(GTATOOLS_GUI_BUILD ON CACHE BOOL "Whether you want to build gtatools-gui")

IF(GTATOOLS_GUI_BUILD)
	SET(CPACK_PACKAGE_EXECUTABLES "gtatools-gui;GTATools GUI Viewer")
ENDIF(GTATOOLS_GUI_BUILD)

INCLUDE(CPack)

SET(ADDITIONAL_INSTALL_FILES "" CACHE STRING "Paths to files which will be additionally installed to the bib subdirectory.")
SET(ADDITIONAL_INSTALL_FILES_ENABLED OFF CACHE BOOL "Set true to enable ADDITIONAL_INSTALL_FILES")


IF(LIBGTAFORMATS_BUILD)
    ADD_SUBDIRECTORY(libgtaformats/src libgtaformats/build)
ENDIF(LIBGTAFORMATS_BUILD)

IF(LIBGTA_BUILD)
    ADD_SUBDIRECTORY(libgta/src libgta/build)
ENDIF(LIBGTA_BUILD)

IF(GTAIMG_BUILD)
    ADD_SUBDIRECTORY(gtaimg/src gtaimg/build)
ENDIF(GTAIMG_BUILD)

IF(GTATXD_BUILD)
    ADD_SUBDIRECTORY(gtatxd/src gtatxd/build)
ENDIF(GTATXD_BUILD)

IF(GTADFF_BUILD)
    ADD_SUBDIRECTORY(gtadff/src gtadff/build)
ENDIF(GTADFF_BUILD)

IF(GTATOOLS_GUI_BUILD)
    ADD_SUBDIRECTORY(gtatools-gui/src gtatools-gui/build)
ENDIF(GTATOOLS_GUI_BUILD)


IF(ADDITIONAL_INSTALL_FILES_ENABLED AND ADDITIONAL_INSTALL_FILES)
    INSTALL(FILES ${ADDITIONAL_INSTALL_FILES} DESTINATION bin)
ENDIF(ADDITIONAL_INSTALL_FILES_ENABLED AND ADDITIONAL_INSTALL_FILES)

