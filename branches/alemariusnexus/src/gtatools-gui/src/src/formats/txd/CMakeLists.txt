# Copyright 2010 David "Alemarius Nexus" Lerch
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
    src/formats/txd/TXDFormatHandler.cpp src/formats/txd/TXDGUIModule.cpp src/formats/txd/TXDWidget.cpp
    src/formats/txd/TextureDisplayer.cpp src/formats/txd/TextureSearchDialog.cpp
    src/formats/txd/OpenTXDGUIModule.cpp
    PARENT_SCOPE)
SET(MOC_SRCS ${MOC_SRCS}
    src/formats/txd/TXDFormatHandler.h src/formats/txd/TXDGUIModule.h src/formats/txd/TXDWidget.h
    src/formats/txd/TextureDisplayer.h src/formats/txd/TextureSearchDialog.h
    src/formats/txd/OpenTXDGUIModule.h
    PARENT_SCOPE)
SET(UIC_SRCS ${UIC_SRCS} src/formats/txd/TXDWidget.ui src/formats/txd/TextureSearchDialog.ui PARENT_SCOPE)
SET(INCS ${INCS} src/formats/txd PARENT_SCOPE)
