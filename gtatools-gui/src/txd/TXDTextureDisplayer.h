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

#ifndef TXDTEXTUREDISPLAYER_H_
#define TXDTEXTUREDISPLAYER_H_

#include <wx/panel.h>
#include <wx/window.h>
#include <wx/scrolwin.h>
#include <gtaformats/txd/TXDTexture.h>

class TXDTextureDisplayer : public wxScrolledWindow {
public:
	TXDTextureDisplayer(wxWindow* parent);
	virtual void OnDraw(wxDC& dc);
	void displayTexture(TXDTexture* texture, uint8_t* data);

private:
	wxBitmap* bitmap;
	wxBitmap* background;
	TXDTexture* texture;
	//uint8_t* data;
};

#endif /* TXDTEXTUREDISPLAYER_H_ */
