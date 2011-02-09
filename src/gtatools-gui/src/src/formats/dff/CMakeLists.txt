# Copyright 2010-2011 David "Alemarius Nexus" Lerch
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

SET(SRCS ${SRCS}
    src/formats/dff/DFFFormatHandler.cpp src/formats/dff/DFFWidget.cpp src/formats/dff/DFFRenderWidget.cpp
    src/formats/dff/DFFGUIModule.cpp src/formats/dff/DFFFrameItemModel.cpp
    src/formats/dff/DFFXMLDumpDialog.cpp
    PARENT_SCOPE)
SET(MOC_SRCS ${MOC_SRCS}
    src/formats/dff/DFFFormatHandler.h src/formats/dff/DFFWidget.h src/formats/dff/DFFRenderWidget.h
    src/formats/dff/DFFGUIModule.h src/formats/dff/DFFFrameItemModel.h src/formats/dff/DFFXMLDumpDialog.h
    PARENT_SCOPE)
SET(UIC_SRCS ${UIC_SRCS} src/formats/dff/DFFWidget.ui src/formats/dff/DFFXMLDumpDialog.ui PARENT_SCOPE)
