/*
 * TXDFormatHandler.cpp
 *
 *  Created on: 11.07.2010
 *      Author: alemariusnexus
 */

#include "DFFFormatHandler.h"
#include <qapplication.h>


DFFFormatHandler::features DFFFormatHandler::getFileFeatures(const File& file, bool fromContent)
{
	if (!file.isDirectory()) {
		if (file.guessContentType() == CONTENT_TYPE_DFF) {
			return FORMAT_HANDLER_DESCRIBE;
		}
	}

	return 0;
}


QString DFFFormatHandler::getFileFormatName(const File& file, bool fromContent)
{
	return tr("DFF Mesh");
}

