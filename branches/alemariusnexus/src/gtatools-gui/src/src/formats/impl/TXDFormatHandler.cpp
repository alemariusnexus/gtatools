/*
 * TXDFormatHandler.cpp
 *
 *  Created on: 11.07.2010
 *      Author: alemariusnexus
 */

#include "TXDFormatHandler.h"
#include "TXDGUIModule.h"



TXDFormatHandler::features TXDFormatHandler::getFileFeatures(const File& file, bool fromContent)
{
	if (!file.isDirectory()) {
		if (file.guessContentType() == CONTENT_TYPE_TXD) {
			return FORMAT_HANDLER_DESCRIBE;
		}
	}

	return 0;
}


QString TXDFormatHandler::getFileFormatName(const File& file, bool fromContent)
{
	return tr("Texture Dictionary (TXD)");
}

