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
    src/formats/dff/DFFFormatHandler.cpp src/formats/dff/DFFWidget.cpp src/formats/dff/DFFRenderWidget.cpp
    src/formats/dff/DFFGUIModule.cpp src/formats/dff/DFFFrameItemModel.cpp
    src/formats/dff/DFFXMLDumpDialog.cpp src/formats/dff/DFFGeometryItemModel.cpp
    src/formats/dff/DFFTextureSourceSelectDialog.cpp src/formats/dff/DFFTextureSourceDialog.cpp
    PARENT_SCOPE)
SET(MOC_SRCS ${MOC_SRCS}
    src/formats/dff/DFFFormatHandler.h src/formats/dff/DFFWidget.h src/formats/dff/DFFRenderWidget.h
    src/formats/dff/DFFGUIModule.h src/formats/dff/DFFFrameItemModel.h src/formats/dff/DFFXMLDumpDialog.h
    src/formats/dff/DFFGeometryItemModel.h src/formats/dff/DFFTextureSourceSelectDialog.h
    src/formats/dff/DFFTextureSourceDialog.h
    PARENT_SCOPE)
SET(UIC_SRCS ${UIC_SRCS}
	src/formats/dff/DFFWidget.ui src/formats/dff/DFFXMLDumpDialog.ui
	src/formats/dff/DFFTextureSourceSelectDialog.ui src/formats/dff/DFFTextureSourceDialog.ui
	PARENT_SCOPE)
