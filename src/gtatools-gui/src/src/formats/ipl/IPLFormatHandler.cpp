/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "IPLFormatHandler.h"
#include <gtaformats/ipl/IPLReader.h>
#include "IPLWidget.h"
#include "../../DefaultDisplayedFile.h"



QString IPLFormatHandler::getFormatName(const File* file) const
{
	if (file) {
		IPLReader ipl(*file);

		if (ipl.isBinary()) {
			return tr("Binary Item Placement File (IPL)");
		}
	}

	return tr("Item Placement File (IPL)");
}


DisplayedFile* IPLFormatHandler::openFile(const FileOpenRequest& request)
{
	IPLWidget* widget = new IPLWidget(NULL, *request.getFile());
	DefaultDisplayedFile* file = new DefaultDisplayedFile(*request.getFile(), this, widget);
	return file;
}
