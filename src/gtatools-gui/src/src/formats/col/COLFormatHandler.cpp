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

#include "COLFormatHandler.h"
#include "COLWidget.h"
#include "../../DefaultDisplayedFile.h"



DisplayedFile* COLFormatHandler::openFile(const FileOpenRequest& request)
{
	COLWidget* widget = new COLWidget(*request.getFile(), NULL);
	DefaultDisplayedFile* file = new DefaultDisplayedFile(*request.getFile(), this, widget);
	return file;
}
