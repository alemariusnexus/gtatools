/*
 * TextureDisplayer.cpp
 *
 *  Created on: 15.07.2010
 *      Author: alemariusnexus
 */

#include "TextureDisplayer.h"
#include <qimage.h>
#include <qbitmap.h>



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

	int16_t w = texture->getWidth();
	int16_t h = texture->getHeight();

	this->texture = texture;
	this->data = new uint8_t[w*h*4];

	texture->convert(data, rawData, MIRROR_NONE, 4, 2, 1, 0, 3);
	QImage image(data, w, h, QImage::Format_ARGB32);
	QPixmap pmp = QPixmap::fromImage(image);
	setPixmap(pmp);
}

