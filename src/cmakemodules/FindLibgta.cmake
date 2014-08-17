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

IF(prj-gta_SOURCE_DIR)
    SET(LIBGTA_LIBRARIES gta)
    SET(LIBGTA_INCLUDE_DIRS "${prj-gta_SOURCE_DIR}" "${prj-gta_BINARY_DIR}")
ELSE(prj-gta_SOURCE_DIR)
    FIND_LIBRARY(LIBGTA_LIBRARIES NAMES gta libgta)
    FIND_PATH(LIBGTA_INCLUDE_DIRS gta/config.h)
ENDIF(prj-gta_SOURCE_DIR)

INCLUDE(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Libgta LIBGTA_LIBRARIES LIBGTA_INCLUDE_DIRS)
