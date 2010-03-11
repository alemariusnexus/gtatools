/*
 * TXDBitmapBuilder.cpp
 *
 *  Created on: 28.02.2010
 *      Author: alemariusnexus
 */

#include "TXDBitmapBuilder.h"
#include <wx/image.h>


wxImage* TXDBitmapBuilder::buildImage(TXDTexture* texture, uint8_t* rawData)
{
	uint16_t w = texture->getWidth(), h = texture->getHeight();

	uint8_t* data = new uint8_t[w * h * 3];
	uint8_t* alpha = new uint8_t[w * h];
	texture->convert(data, rawData, MIRROR_NONE, 3, 0, 1, 2, -1);
	texture->convert(alpha, rawData, MIRROR_NONE, 1, -1, -1, -1, 0);

	wxImage* image = new wxImage(w, h, data, false);
	image->SetAlpha(alpha, false);
	return image;
}


wxBitmap* TXDBitmapBuilder::buildBitmap(TXDTexture* texture, uint8_t* rawData)
{
	wxImage* img = buildImage(texture, rawData);
	wxBitmap* bmp = new wxBitmap(*img);
	delete img;
	return bmp;
}
