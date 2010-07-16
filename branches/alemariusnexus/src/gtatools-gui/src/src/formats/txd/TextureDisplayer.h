/*
 * TextureDisplayer.h
 *
 *  Created on: 15.07.2010
 *      Author: alemariusnexus
 */

#ifndef TEXTUREDISPLAYER_H_
#define TEXTUREDISPLAYER_H_

#include <qlabel.h>
#include <gtaformats/txd/TXDTexture.h>



class TextureDisplayer : public QLabel {
	Q_OBJECT

public:
	TextureDisplayer(QWidget* parent);
	TextureDisplayer(TXDTexture* texture, uint8_t* rawData, QWidget* parent);
	~TextureDisplayer();
	void display(TXDTexture* texture, uint8_t* rawData);

private:
	TXDTexture* texture;
	uint8_t* data;
};

#endif /* TEXTUREDISPLAYER_H_ */
