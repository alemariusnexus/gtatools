/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#ifndef TEXTUREDISPLAYER_H_
#define TEXTUREDISPLAYER_H_

#include <QtGui/QLabel>
#include <gtaformats/txd/TXDTexture.h>
#include <QtGui/QMouseEvent>


class TextureDisplayer : public QLabel {
	Q_OBJECT

public:
	TextureDisplayer(QWidget* parent);
	TextureDisplayer(TXDTexture* texture, uint8_t* rawData, QWidget* parent);
	~TextureDisplayer();
	void display(TXDTexture* texture, uint8_t* rawData);

protected:
	virtual void mouseMoveEvent(QMouseEvent* evt);

private:
	TXDTexture* texture;
	uint8_t* data;
};

#endif /* TEXTUREDISPLAYER_H_ */
