/*
	Copyright 2010 David Lerch

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
