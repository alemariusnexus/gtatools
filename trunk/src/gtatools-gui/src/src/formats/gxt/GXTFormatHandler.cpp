/*
	Copyright 2010 David "Alemarius Nexus" Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "GXTFormatHandler.h"
#include "GXTWidget.h"



bool GXTFormatHandler::hasFileFormat(const File& file) const
{
	return QString(file.getPath()->getExtension()).toLower().compare("gxt") == 0;
}


QWidget* GXTFormatHandler::createWidgetForFile(const FileOpenRequest& request, QWidget* parent)
{
	GXTWidget* widget = new GXTWidget;
	widget->addFile(*request.getFile());
	return widget;
}
