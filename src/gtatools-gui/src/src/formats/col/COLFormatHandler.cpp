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

#include "COLFormatHandler.h"
#include "COLWidget.h"
#include "../../DisplayedFile.h"
#include "../../System.h"



bool COLFormatHandler::canHandle(const EntityOpenRequest& req) const
{
	QVariant fileVar = req.getAttribute("file");

	if (fileVar.isNull())
		return false;

	File file(fileVar.toString().toLocal8Bit().constData());

	return file.guessContentType() == CONTENT_TYPE_COL;
}


DisplayedEntity* COLFormatHandler::openEntity(const EntityOpenRequest& request)
{
	COLWidget* widget;

	File file(request.getAttribute("file").toString().toLocal8Bit().constData());

	try {
		widget = new COLWidget(file, NULL);
	} catch (Exception& ex) {
		System::getInstance()->log(LogEntry::error(QString(tr("Error opening COL file: %1"))
				.arg(ex.getMessage()), &ex));
		return NULL;
	}

	DisplayedFile* dfile = new DisplayedFile(file, this, widget);
	return dfile;
}
