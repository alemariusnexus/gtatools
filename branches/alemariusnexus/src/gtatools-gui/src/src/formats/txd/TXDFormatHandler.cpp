/*
 * TXDFormatHandler.cpp
 *
 *  Created on: 11.07.2010
 *      Author: alemariusnexus
 */

#include "TXDFormatHandler.h"
#include "TXDGUIModule.h"
#include "TXDWidget.h"
#include <cstdio>



TXDFormatHandler::features TXDFormatHandler::getFileFeatures(const File& file, bool fromContent)
{
	if (!file.isDirectory()) {
		if (file.guessContentType() == CONTENT_TYPE_TXD) {
			return FORMAT_HANDLER_DESCRIBE | FORMAT_HANDLER_WIDGET;
		}
	}

	return 0;
}


QString TXDFormatHandler::getFileFormatName(const File& file, bool fromContent)
{
	return tr("Texture Dictionary (TXD)");
}


QWidget* TXDFormatHandler::createWidgetForFile(const File& file, QWidget* parent)
{
	printf("Creating widget\n");
	return new TXDWidget(file, parent);
}

