# Copyright 2010-2012 David "Alemarius Nexus" Lerch
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
    src/formats/ifp/IFPWidget.cpp src/formats/ifp/IFPFormatHandler.cpp src/formats/ifp/IFPGUIModule.cpp
    src/formats/ifp/IFPAnimationViewerWidget.cpp src/formats/ifp/IFPAnimationViewerEntity.cpp
    src/formats/ifp/IFPAnimationViewerRenderWidget.cpp src/formats/ifp/IFPFrameItemModel.cpp
    PARENT_SCOPE)
SET(MOC_SRCS ${MOC_SRCS}
    src/formats/ifp/IFPWidget.h src/formats/ifp/IFPFormatHandler.h src/formats/ifp/IFPGUIModule.h
    src/formats/ifp/IFPAnimationViewerWidget.h src/formats/ifp/IFPAnimationViewerEntity.h
    src/formats/ifp/IFPAnimationViewerRenderWidget.h src/formats/ifp/IFPFrameItemModel.h
    PARENT_SCOPE)
SET(UIC_SRCS ${UIC_SRCS}
    src/formats/ifp/IFPWidget.ui src/formats/ifp/IFPAnimationViewerWidget.ui
    PARENT_SCOPE)
