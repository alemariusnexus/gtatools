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
		dc.DrawBitmap(*bitmap, 0, 0, false);
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

	uint8_t* data = new uint8_t[texture->getWidth() * texture->getHeight() * 3];
	texture->convert(data, rawData, MIRROR_NONE, 3, 0, 1, 2, -1);

	wxImage image(w, h, data, false);
	bitmap = new wxBitmap(image);

	Refresh();
}
