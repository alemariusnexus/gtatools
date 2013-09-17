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

SET(SRCS ${SRCS}
    src/formats/ide/IDEFormatHandler.cpp src/formats/ide/IDEWidget.cpp src/formats/ide/IDEGUIModule.cpp
    src/formats/ide/IDESearchWidget.cpp src/formats/ide/IDEFileFinder.cpp
    PARENT_SCOPE)
SET(MOC_SRCS ${MOC_SRCS}
    src/formats/ide/IDEFormatHandler.h src/formats/ide/IDEWidget.h src/formats/ide/IDEGUIModule.h
    src/formats/ide/IDESearchWidget.h
    PARENT_SCOPE)
SET(UIC_SRCS ${UIC_SRCS}
    src/formats/ide/IDEWidget.ui src/formats/ide/IDESearchWidget.ui
    PARENT_SCOPE)
