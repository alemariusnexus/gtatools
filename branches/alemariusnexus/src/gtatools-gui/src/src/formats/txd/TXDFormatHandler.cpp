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

#include "TXDFormatHandler.h"
#include "TXDGUIModule.h"
#include "TXDWidget.h"
#include <cstdio>




TXDFormatHandler* TXDFormatHandler::getInstance()
{
	static TXDFormatHandler* inst = new TXDFormatHandler;
	return inst;
}


QWidget* TXDFormatHandler::createWidgetForFile(const File& file, QWidget* parent, const QHash<QString, QVariant>& data)
{
	QString texName;

	if (data.contains("texture"))  {
		texName = data["texture"].toString();
	}

	return new TXDWidget(file, texName, parent);
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

