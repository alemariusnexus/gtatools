/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "TextureDisplayer.h"
#include <QtGui/QImage>
#include <QtGui/QBitmap>
#include "TXDFormatHandler.h"
#include "../../System.h"



TextureDisplayer::TextureDisplayer(TXDTextureHeader* texture, uint8_t* rawData, QWidget* parent)
		: QLabel(parent), texture(NULL), data(NULL)
{
	display(texture, rawData);
	setMouseTracking(true);
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


void TextureDisplayer::display(TXDTextureHeader* texture, uint8_t* rawData)
{
	if (data) {
		delete[] data;
	}

	this->texture = texture;

	if (texture) {
		QImage image = TXDFormatHandler::getInstance()->createImageFromTexture(texture, rawData, this->data);

		QPixmap pmp = QPixmap::fromImage(image);
		setPixmap(pmp);
	} else {
		clear();
		data = NULL;
	}
}


void TextureDisplayer::mouseMoveEvent(QMouseEvent* evt)
{
	// TODO Make this work...
	const QPoint pos = evt->pos();

	if (texture  &&  data) {
		if (	pos.x() >= 0  &&  pos.y() >= 0  &&  pos.x() <= texture->getWidth()
				&&  pos.y() <= texture->getHeight()
		) {
			int offs = (pos.y()*texture->getWidth() + pos.x()) * 4;

			System::getInstance()->showStatusMessage(QString("Pixel: (%1, %2), RGBA: (%3, %4, %5; %6)")
					.arg(pos.x()).arg(pos.y()).arg(data[offs]).arg(data[offs+1]).arg(data[offs+2])
					.arg(data[offs+3]), 0);
		}
	}
}
