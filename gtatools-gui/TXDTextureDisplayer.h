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
#include <txd/TXDTexture.h>

class TXDTextureDisplayer : public wxWindow {
public:
	TXDTextureDisplayer(wxWindow* parent);
	virtual void OnPaint(wxPaintEvent& evt);
	void displayTexture(TXDTexture* texture, uint8_t* data);

private:
	wxBitmap* bitmap;
	wxBitmap* background;
	TXDTexture* texture;
	//uint8_t* data;
};

#endif /* TXDTEXTUREDISPLAYER_H_ */
