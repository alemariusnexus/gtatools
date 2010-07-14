/*
 * TXDFormatHandler.cpp
 *
 *  Created on: 11.07.2010
 *      Author: alemariusnexus
 */

#include "IDEFormatHandler.h"


IDEFormatHandler::features IDEFormatHandler::getFileFeatures(const File& file, bool fromContent)
{
	if (!file.isDirectory()) {
		if (file.guessContentType() == CONTENT_TYPE_IDE) {
			return FORMAT_HANDLER_DESCRIBE;
		}
	}

	return 0;
}


QString IDEFormatHandler::getFileFormatName(const File& file, bool fromContent)
{
	return tr("Item Definition File (IDE)");
}

