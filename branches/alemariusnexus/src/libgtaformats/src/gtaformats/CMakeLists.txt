# Copyright 2010 David "Alemarius Nexus"
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


SET(FILES gta GTASectionFileReader)

FOREACH(FILE ${FILES})
    SET(SRCS ${SRCS} "gtaformats/${FILE}")
ENDFOREACH(FILE)

CONFIGURE_FILE(gf_config.h.cmake "${CMAKE_CURRENT_SOURCE_DIR}/gf_config.h")

ADD_SUBDIRECTORY(dff)
ADD_SUBDIRECTORY(engine)
ADD_SUBDIRECTORY(gxt)
ADD_SUBDIRECTORY(ide)
ADD_SUBDIRECTORY(ipl)
ADD_SUBDIRECTORY(img)
ADD_SUBDIRECTORY(txd)
ADD_SUBDIRECTORY(util)

SET(SRCS ${SRCS} PARENT_SCOPE)


#INSTALL(FILES gf_config.h DESTINATION include/gtaformats)
