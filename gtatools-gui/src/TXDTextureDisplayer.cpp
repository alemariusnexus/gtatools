/*
 * TXDTextureDisplayer.cpp
 *
 *  Created on: 24.01.2010
 *      Author: alemariusnexus
 */

#include "TXDTextureDisplayer.h"
#include <wx/wx.h>

TXDTextureDisplayer::TXDTextureDisplayer(wxWindow* parent)
		: wxWindow(parent, -1), bitmap(NULL)
{
	Connect(wxEVT_PAINT, wxPaintEventHandler(TXDTextureDisplayer::OnPaint));
}


void TXDTextureDisplayer::OnPaint(wxPaintEvent& evt)
{
	if (bitmap) {
		wxPaintDC dc(this);
		dc.DrawBitmap(*bitmap, 0, 0, true);
	}
}


void TXDTextureDisplayer::displayTexture(TXDTexture* texture, uint8_t* rawData)
{
	if (bitmap) {
		delete bitmap;
		bitmap = NULL;
	}

	this->texture = texture;

	uint16_t w = texture->getWidth(), h = texture->getHeight();

	uint8_t* data = new uint8_t[w * h * 3];
	uint8_t* alpha = new uint8_t[w * h];
	texture->convert(data, rawData, MIRROR_NONE, 3, 0, 1, 2, -1);
	texture->convert(alpha, rawData, MIRROR_NONE, 1, -1, -1, -1, 0);

	wxImage image(w, h, data, false);
	image.SetAlpha(alpha, false);
	bitmap = new wxBitmap(image);


	uint8_t* bgData = new uint8_t[w * h * 3];

	for (int16_t y = 0 ; y < h ; y++) {
		for (int16_t x = 0 ; x < w ; x++) {
			uint8_t pixval = ((y+x) % 2) * 255;

			bgData[((y*w + x) * 3)] = pixval;
			bgData[((y*w + x) * 3) + 1] = pixval;
			bgData[((y*w + x) * 3) + 2] = pixval;
		}
	}

	Refresh();
}
