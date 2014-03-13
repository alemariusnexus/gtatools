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

#include "IMGFormatHandler.h"
#include <gtaformats/img/IMGArchive.h>
#include "../../System.h"



IMGFormatHandler::IMGFormatHandler()
		: guiModule(new IMGGUIModule)
{
	System* sys = System::getInstance();
	sys->installGUIModule(guiModule);
}


IMGFormatHandler::~IMGFormatHandler()
{
	delete guiModule;
}


QString IMGFormatHandler::getFileFormatName(const File& file) const
{
	FileContentType type = file.guessContentType();

	if (type == CONTENT_TYPE_DIR) {
		return tr("IMG Archive Header (DIR)");
	} else {
		IMGArchive::IMGVersion ver = IMGArchive::guessIMGVersion(file);

		if (ver == IMGArchive::VER1) {
			return tr("IMG Archive Version 1 (GTA III/VC)");
		} else {
			return tr("IMG Archive Version 2 (GTA SA)");
		}
	}
}


bool IMGFormatHandler::canHandle(const EntityOpenRequest& req) const
{
	QVariant typeVar = req.getAttribute("type");

	if (typeVar.isNull())
		return false;

	if (typeVar.toString() != "file")
		return false;

	QVariant fileVar = req.getAttribute("file");

	if (fileVar.isNull())
		return false;

	File file(fileVar.toString().toLocal8Bit().constData());

	return	file.guessContentType() == CONTENT_TYPE_IMG
			||  file.guessContentType() == CONTENT_TYPE_DIR;
}
