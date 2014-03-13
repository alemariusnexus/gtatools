# Copyright 2010-2014 David "Alemarius Nexus" Lerch
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

# Additional permissions are granted, which are listed in the file
# GPLADDITIONS.

SET(SRCS ${SRCS}
    src/formats/rwbs/script/RWBSScriptByteReader.cpp src/formats/rwbs/script/ScriptRWSection.cpp
    src/formats/rwbs/script/ScriptRWBSBlock.cpp
    PARENT_SCOPE)
SET(MOC_SRCS ${MOC_SRCS}
    PARENT_SCOPE)
SET(UIC_SRCS ${UIC_SRCS}
    PARENT_SCOPE)
SET(INCS ${INCS} src/formats/rwbs/script PARENT_SCOPE)
