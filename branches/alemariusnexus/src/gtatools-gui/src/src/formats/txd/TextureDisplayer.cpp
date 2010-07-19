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

#include "TextureDisplayer.h"
#include <qimage.h>
#include <qbitmap.h>
#include "TXDFormatHandler.h"



TextureDisplayer::TextureDisplayer(TXDTexture* texture, uint8_t* rawData, QWidget* parent)
		: QLabel(parent), texture(NULL), data(NULL)
{
	display(texture, rawData);
}


TextureDisplayer::TextureDisplayer(QWidget* parent)
		: QLabel(parent), texture(NULL), data(NULL)
{
}


TextureDisplayer::~TextureDisplayer()
{
	if (data) {
		delete[] data;
	}
}


void TextureDisplayer::display(TXDTexture* texture, uint8_t* rawData)
{
	if (data) {
		delete[] data;
	}

	this->texture = texture;

	QImage image = TXDFormatHandler::getInstance()->createImageFromTexture(texture, rawData, this->data);

	QPixmap pmp = QPixmap::fromImage(image);
	setPixmap(pmp);
}

