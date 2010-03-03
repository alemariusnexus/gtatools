/*
 * TXDTextureDisplayer.cpp
 *
 *  Created on: 24.01.2010
 *      Author: alemariusnexus
 */

#include "TXDTextureDisplayer.h"
#include "TXDBitmapBuilder.h"
#include <wx/wx.h>
#include <cstdio>

TXDTextureDisplayer::TXDTextureDisplayer(wxWindow* parent)
		: wxScrolledWindow(parent, -1), bitmap(NULL)
{
	SetScrollRate(5, 5);
}


void TXDTextureDisplayer::OnDraw(wxDC& dc)
{
	if (bitmap) {
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
	SetVirtualSize(texture->getWidth(), texture->getHeight());
	SetMinSize(wxSize(5, 5));

	bitmap = TXDBitmapBuilder().buildBitmap(texture, rawData);

	Refresh();
}
