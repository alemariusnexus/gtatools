/*
 * IMGFormatHandler.cpp
 *
 *  Created on: 10.07.2010
 *      Author: alemariusnexus
 */

#include "IMGFormatHandler.h"
#include <gtaformats/img/IMGArchive.h>



IMGFormatHandler::features IMGFormatHandler::getFileFeatures(const File& file, bool fromContent)
{
	FileContentType type = file.guessContentType();

	if (type == CONTENT_TYPE_DIR  ||  type == CONTENT_TYPE_IMG) {
		return FORMAT_HANDLER_DESCRIBE;
	} else {
		return 0;
	}
}


QString IMGFormatHandler::getFileFormatName(const File& file, bool fromContent)
{
	FileContentType type = file.guessContentType();

	if (type == CONTENT_TYPE_DIR) {
		return "IMG Archive Header (DIR)";
	} else {
		if (fromContent) {
			IMGArchive::IMGVersion ver = IMGArchive::guessIMGVersion(file);

			if (ver == IMGArchive::VER1) {
				return "IMG Archive Version 1 (GTA III/VC)";
			} else {
				return "IMG Archive Version 2 (GTA SA)";
			}
		} else {
			return "IMG Archive";
		}
	}
}
