/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#include "IFPFormatHandler.h"
#include "IFPWidget.h"
#include "IFPAnimationViewerWidget.h"
#include "IFPAnimationViewerEntity.h"
#include "../../DisplayedFile.h"
#include "../../System.h"



IFPFormatHandler::IFPFormatHandler()
{
	guiModule = new IFPGUIModule;
	System::getInstance()->installGUIModule(guiModule);
}


bool IFPFormatHandler::canHandle(const EntityOpenRequest& req) const
{
	QString type = req.getAttribute("type").toString();

	if (type == "file") {
		QVariant fileVar = req.getAttribute("file");

		if (fileVar.isNull())
			return false;

		File file(fileVar.toString().toLocal8Bit().constData());

		return file.guessContentType() == CONTENT_TYPE_IFP;
	} else if (type == "IFP::AnimViewer") {
		return true;
	}

	return false;
}


DisplayedEntity* IFPFormatHandler::openEntity(const EntityOpenRequest& request)
{
	QString type = request.getAttribute("type").toString();

	if (type == "file") {
		File file(request.getAttribute("file").toString().toLocal8Bit().constData());

		IFPWidget* widget = new IFPWidget(file);

		DisplayedFile* dfile = new DisplayedFile(file, this, widget);

		return dfile;
	} else {
		IFPAnimationViewerWidget* viewer = new IFPAnimationViewerWidget;
		IFPAnimationViewerEntity* ent = new IFPAnimationViewerEntity(viewer);

		return ent;
	}
}
