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




TXDFormatHandler* TXDFormatHandler::getInstance()
{
	static TXDFormatHandler* inst = new TXDFormatHandler;
	return inst;
}


QWidget* TXDFormatHandler::createWidgetForFile(const File& file, QWidget* parent)
{
	return new TXDWidget(file, parent);
}


QImage TXDFormatHandler::createImageFromTexture(TXDTexture* tex, uint8_t* data, uint8_t*& resultData)
{
	int16_t w = tex->getWidth();
	int16_t h = tex->getHeight();

	resultData = new uint8_t[w*h*4];

	tex->convert(resultData, data, MIRROR_NONE, 4, 2, 1, 0, 3);
	QImage image(resultData, w, h, QImage::Format_ARGB32);

	return image;
}

