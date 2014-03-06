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
    src/gui/FileItemModel.cpp src/gui/StaticFile.cpp src/gui/MainWindow.cpp src/gui/ProfileConfigWidget.cpp
    src/gui/ConfigWidget.cpp src/gui/GUIModule.cpp src/gui/DefaultGUIModule.cpp src/gui/FileSearchDialog.cpp
    src/gui/VersionDialog.cpp src/gui/FileTree.cpp src/gui/FileViewWidget.cpp src/gui/LogConsole.cpp
    src/gui/GUI.cpp src/gui/GLBaseWidget.cpp src/gui/HexEditor.cpp
    src/gui/HexEditorPrivate.cpp src/gui/HexEditorDecoder.cpp src/gui/HexEditorGotoDialog.cpp
    src/gui/GLContainerWidget.cpp src/gui/PVSDialog.cpp src/gui/ImageDisplayLabel.cpp
    src/gui/ImageDisplayWidget.cpp src/gui/HTMLItemViewDelegate.cpp src/gui/HexEditorUndoCommand.cpp
    src/gui/HexEditorDocument.cpp src/gui/HexEditorBlock.cpp src/gui/HexEditorContentTraverser.cpp
    PARENT_SCOPE)
SET(MOC_SRCS ${MOC_SRCS}
    src/gui/FileItemModel.h src/gui/MainWindow.h src/gui/ProfileConfigWidget.h src/gui/ConfigWidget.h
    src/gui/GUIModule.h src/gui/DefaultGUIModule.h src/gui/FileSearchDialog.h src/gui/VersionDialog.h
    src/gui/FileTree.h src/gui/FileViewWidget.h src/gui/LogConsole.h src/gui/GUI.h src/gui/GLBaseWidget.h
    src/gui/HexEditor.h src/gui/HexEditorPrivate.h src/gui/HexEditorDecoder.h
    src/gui/HexEditorGotoDialog.h src/gui/GLContainerWidget.h src/gui/GLContainerWidgetPrivate.h
    src/gui/PVSDialog.h src/gui/ImageDisplayLabel.h src/gui/ImageDisplayWidget.h src/gui/PercentageValidator.h
    src/gui/FixedSizeLineEdit.h src/gui/HTMLItemViewDelegate.h src/gui/HexEditorUndoCommand.h
    src/gui/HexEditorDocument.h src/gui/HexEditorBlock.h src/gui/HexEditorContentTraverser.h
    PARENT_SCOPE)
SET(UIC_SRCS ${UIC_SRCS}
    src/gui/MainFrame.ui src/gui/ProfileConfigWidget.ui src/gui/ConfigWidget.ui src/gui/FileSearchDialog.ui
    src/gui/VersionDialog.ui src/gui/FileViewWidget.ui src/gui/LogConsole.ui
    src/gui/HexEditorDecoder.ui src/gui/HexEditorGotoDialog.ui src/gui/GLContainerWidget.ui
    src/gui/PVSDialog.ui
    PARENT_SCOPE)
SET(INCS ${INCS} src/gui PARENT_SCOPE)
