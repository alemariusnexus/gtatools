/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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
#include "../../DisplayedFile.h"



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


bool IPLFormatHandler::canHandle(const EntityOpenRequest& req) const
{
	QVariant fileVar = req.getAttribute("file");

	if (fileVar.isNull())
		return false;

	File file(fileVar.toString().toLocal8Bit().constData());

	return file.guessContentType() == CONTENT_TYPE_IPL;
}


DisplayedEntity* IPLFormatHandler::openEntity(const EntityOpenRequest& request)
{
	File file(request.getAttribute("file").toString().toLocal8Bit().constData());
	IPLWidget* widget = new IPLWidget(NULL, file);
	DisplayedFile* dfile = new DisplayedFile(file, this, widget);
	return dfile;
}
