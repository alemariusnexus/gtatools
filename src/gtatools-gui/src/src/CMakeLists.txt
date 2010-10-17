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

SET(CONFIG_DIR "/etc/gtatools-gui" CACHE PATH "The configuration directory of gtatools-gui")

ADD_SUBDIRECTORY(gui)
ADD_SUBDIRECTORY(formats)

CONFIGURE_FILE(config.cmake.h "${CMAKE_CURRENT_SOURCE_DIR}/config.h")

SET(SRCS ${SRCS}
    src/main.cpp src/Profile.cpp src/ProfileManager.cpp src/System.cpp src/ProfileInitializer.cpp
    src/Task.cpp src/FileOpenRequest.cpp src/RegexFileFinder.cpp src/TaskFileFinder.cpp
    PARENT_SCOPE)
SET(MOC_SRCS ${MOC_SRCS}
    src/ProfileManager.h src/Profile.h src/System.h src/Task.h src/ProfileInitializer.h
    PARENT_SCOPE)
SET(UIC_SRCS ${UIC_SRCS} PARENT_SCOPE)
SET(INCS ${INCS} PARENT_SCOPE)
