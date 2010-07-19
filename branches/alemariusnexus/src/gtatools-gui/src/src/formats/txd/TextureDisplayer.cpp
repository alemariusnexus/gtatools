/*
 * TextureDisplayer.cpp
 *
 *  Created on: 15.07.2010
 *      Author: alemariusnexus
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

