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

#include "IDEFormatHandler.h"
#include "IDEWidget.h"
#include "IDEFileFinder.h"
#include "../../gui/GUI.h"
#include "../../System.h"
#include "../../DefaultDisplayedFile.h"



IDEFormatHandler::IDEFormatHandler()
{
	connect(System::getInstance(), SIGNAL(systemQuerySent(const SystemQuery&, SystemQueryResult&)), this,
			SLOT(systemQuerySent(const SystemQuery&, SystemQueryResult&)));
}


DisplayedFile* IDEFormatHandler::openFile(const FileOpenRequest& request)
{
	IDEWidget* widget = new IDEWidget(NULL, request);
	DefaultDisplayedFile* file = new DefaultDisplayedFile(*request.getFile(), this, widget);
	return file;
}


void IDEFormatHandler::systemQuerySent(const SystemQuery& query, SystemQueryResult& result)
{
	if (!result.isSuccessful()) {
		if (query.getName() == "FindItemDefinition") {
			int id = query["id"].toInt();

			IDEFileFinder finder(id, IDETypeGroupAll);
			File* toBeOpened = GUI::getInstance()->findFile(&finder);

			if (toBeOpened) {
				int line = finder.getMatchedLine(*toBeOpened);
				result["file"] = toBeOpened->getPath()->toString();
				result["line"] = line;
				delete toBeOpened;
				result.setSuccessful(true);
			}
		}
	}
}
