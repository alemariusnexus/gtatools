/*
 * IMGFormatHandler.cpp
 *
 *  Created on: 10.07.2010
 *      Author: alemariusnexus
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
