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

#include "IMGFormatHandler.h"
#include <gtaformats/img/IMGArchive.h>




QString IMGFormatHandler::getFormatName(const File* file) const
{
	if (!file) {
		return tr("IMG Archive");
	}

	FileContentType type = file->guessContentType();

	if (type == CONTENT_TYPE_DIR) {
		return tr("IMG Archive Header (DIR)");
	} else {
		IMGArchive::IMGVersion ver = IMGArchive::guessIMGVersion(*file);

		if (ver == IMGArchive::VER1) {
			return tr("IMG Archive Version 1 (GTA III/VC)");
		} else {
			return tr("IMG Archive Version 2 (GTA SA)");
		}
	}
}