# Copyright 2010-2013 David "Alemarius Nexus" Lerch
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

ADD_SUBDIRECTORY(unrecog)
#ADD_SUBDIRECTORY(col)
ADD_SUBDIRECTORY(dff)
ADD_SUBDIRECTORY(ide)
#ADD_SUBDIRECTORY(ifp)
ADD_SUBDIRECTORY(img)
ADD_SUBDIRECTORY(ipl)
ADD_SUBDIRECTORY(txd)
ADD_SUBDIRECTORY(gxt)
ADD_SUBDIRECTORY(rwbs)

SET(SRCS ${SRCS} src/formats/EntityHandler.cpp src/formats/EntityManager.cpp PARENT_SCOPE)
SET(MOC_SRCS ${MOC_SRCS} src/formats/EntityHandler.h PARENT_SCOPE)
SET(UIC_SRCS ${UIC_SRCS} PARENT_SCOPE)
SET(INCS ${INCS} PARENT_SCOPE)
