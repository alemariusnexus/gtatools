/*
 * TXDFormatHandler.cpp
 *
 *  Created on: 11.07.2010
 *      Author: alemariusnexus
 */

#include "IPLFormatHandler.h"


IPLFormatHandler::features IPLFormatHandler::getFileFeatures(const File& file, bool fromContent)
{
	if (!file.isDirectory()) {
		if (file.guessContentType() == CONTENT_TYPE_IPL) {
			return FORMAT_HANDLER_DESCRIBE;
		}
	}

	return 0;
}


QString IPLFormatHandler::getFileFormatName(const File& file, bool fromContent)
{
	return tr("Item Placement File (IPL)");
}

