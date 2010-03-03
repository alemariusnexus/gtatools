/*
 * TXDTextureDisplayer.h
 *
 *  Created on: 24.01.2010
 *      Author: alemariusnexus
 */

#ifndef TXDTEXTUREDISPLAYER_H_
#define TXDTEXTUREDISPLAYER_H_

#include <wx/panel.h>
#include <wx/window.h>
#include <wx/scrolwin.h>
#include <txd/TXDTexture.h>

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
