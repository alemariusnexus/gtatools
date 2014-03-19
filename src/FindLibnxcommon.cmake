# Copyright 2010-2014 David "Alemarius Nexus" Lerch
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
#
# Additional permissions are granted, which are listed in the file
# GPLADDITIONS.

IF(nxcommon_SOURCE_DIR)
    MESSAGE(STATUS "Building libnxcommon along with gtatools in ${nxcommon_SOURCE_DIR}")
    SET(LIBNXCOMMON_LIBRARIES nxcommon CACHE FILEPATH "libnxcommon library file")
    SET(LIBNXCOMMON_INCLUDE_DIRS "${nxcommon_SOURCE_DIR}" "${nxcommon_BINARY_DIR}" "${nxcommon_SOURCE_DIR}/nxcommon" CACHE PATH "libnxcommon include directories")
ELSE(nxcommon_SOURCE_DIR)
    MESSAGE(STATUS "Using external libnxcommon (to build it along with gtatools, place the nxcommon root folder as nxcommon in ${CMAKE_SOURCE_DIR}")
    FIND_LIBRARY(LIBNXCOMMON_LIBRARIES NAMES nxcommon libnxcommon)
    FIND_PATH(LIBNXCOMMON_INCLUDE_DIRS nxcommon/config.h)
ENDIF(nxcommon_SOURCE_DIR)

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Libnxcommon LIBNXCOMMON_LIBRARIES LIBNXCOMMON_INCLUDE_DIRS)
