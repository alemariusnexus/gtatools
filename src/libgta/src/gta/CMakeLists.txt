# Copyright 2010-2012 David "Alemarius Nexus"
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


SET(FILES
    gl Camera Shader ShaderProgram ItemDefinition GLException Engine MeshGenerator ItemManager
    MapItemDefinition StaticMapItemDefinition TimedMapItemDefinition GameInfo
    Animation AnimationBone AnimationFrame AnimatedMapItemDefinition Animator gldebug)
    
ADD_SUBDIRECTORY(resource)
ADD_SUBDIRECTORY(scene)
#ADD_SUBDIRECTORY(ceguigl2)
    
#CONFIGURE_FILE(config.cmake.h "${CMAKE_CURRENT_SOURCE_DIR}/config.h")
CONFIGURE_FILE(config.cmake.h "${CMAKE_CURRENT_BINARY_DIR}/config.h")

FOREACH(FILE ${FILES})
    SET(SRCS ${SRCS} "gta/${FILE}")
ENDFOREACH(FILE)

SET(SRCS ${SRCS} PARENT_SCOPE)

