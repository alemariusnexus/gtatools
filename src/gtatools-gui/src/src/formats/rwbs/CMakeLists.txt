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

ADD_SUBDIRECTORY(script)

SET(SRCS ${SRCS}
    src/formats/rwbs/RWBSWidget.cpp src/formats/rwbs/RWBSSectionModel.cpp src/formats/rwbs/RWBSGUIModule.cpp
    src/formats/rwbs/RWBSSectionTree.cpp src/formats/rwbs/RWBSSectionDialog.cpp
    src/formats/rwbs/RWBSUtilityWidget.cpp src/formats/rwbs/RWBSAnalyzer.cpp src/formats/rwbs/RWBSAnalyzerWidget.cpp
    src/formats/rwbs/RWBSAnalyzerWidget.cpp
    PARENT_SCOPE)
SET(MOC_SRCS ${MOC_SRCS}
    src/formats/rwbs/RWBSWidget.h src/formats/rwbs/RWBSSectionModel.h src/formats/rwbs/RWBSGUIModule.h
    src/formats/rwbs/RWBSSectionTree.h src/formats/rwbs/RWBSSectionDialog.h src/formats/rwbs/RWBSUtilityWidget.h
    src/formats/rwbs/RWBSAnalyzer.h src/formats/rwbs/RWBSAnalyzerWidget.h src/formats/rwbs/RWBSAnalyzerWidget.h
    PARENT_SCOPE)
SET(UIC_SRCS ${UIC_SRCS}
    src/formats/rwbs/RWBSWidget.ui src/formats/rwbs/RWBSSectionDialog.ui src/formats/rwbs/RWBSAnalyzerWidget.ui
    PARENT_SCOPE)
SET(INCS ${INCS} src/formats/rwbs PARENT_SCOPE)
