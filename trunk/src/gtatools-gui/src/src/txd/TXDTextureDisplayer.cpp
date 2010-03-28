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
