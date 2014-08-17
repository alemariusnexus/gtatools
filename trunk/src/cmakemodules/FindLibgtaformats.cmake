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

IF(prj-gtaformats_SOURCE_DIR)
    SET(LIBGTAFORMATS_LIBRARIES gtaformats)
    SET(LIBGTAFORMATS_INCLUDE_DIRS "${prj-gtaformats_SOURCE_DIR}" "${prj-gtaformats_BINARY_DIR}" "${prj-gtaformats_SOURCE_DIR}/gtaformats")
ELSE(prj-gtaformats_SOURCE_DIR)
    FIND_LIBRARY(LIBGTAFORMATS_LIBRARIES NAMES gtaformats libgtaformats)
    FIND_PATH(LIBGTAFORMATS_INCLUDE_DIRS gtaformats/config.h)
ENDIF(prj-gtaformats_SOURCE_DIR)

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Libgtaformats LIBGTAFORMATS_LIBRARIES LIBGTAFORMATS_INCLUDE_DIRS)
