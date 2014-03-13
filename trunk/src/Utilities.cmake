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

MACRO(ADD_SOURCES)
    FOREACH(_SRC ${ARGV})
        SET(SRCS ${SRCS} "${CMAKE_CURRENT_SOURCE_DIR}/${_SRC}")
    ENDFOREACH(_SRC)
    
    SET(SRCS ${SRCS} PARENT_SCOPE)
ENDMACRO(ADD_SOURCES)
