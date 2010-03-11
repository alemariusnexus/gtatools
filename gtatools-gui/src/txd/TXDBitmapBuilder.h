/*
 * TXDBitmapBuilder.h
 *
 *  Created on: 28.02.2010
 *      Author: alemariusnexus
 */

#ifndef TXDBITMAPBUILDER_H_
#define TXDBITMAPBUILDER_H_

#include <txd/TXDTexture.h>
#include <wx/bitmap.h>
#include <wx/image.h>


class TXDBitmapBuilder
{
public:
	wxImage* buildImage(TXDTexture* texture, uint8_t* rawData);
	wxBitmap* buildBitmap(TXDTexture* texture, uint8_t* rawData);
};

#endif /* TXDBITMAPBUILDER_H_ */
