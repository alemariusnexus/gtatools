/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef IMAGEDISPLAYLABEL_H_
#define IMAGEDISPLAYLABEL_H_

#include <QtGui/QLabel>
#include <QtGui/QMouseEvent>
#include <QtGui/QResizeEvent>
#include <QtGui/QTransform>


class ImageDisplayLabel : public QLabel {
	Q_OBJECT

public:
	ImageDisplayLabel(QWidget* parent);
	ImageDisplayLabel(const QPixmap& pixmap, QWidget* parent);
	~ImageDisplayLabel();
	void display(const QPixmap& pixmap);
	void setImageAutoScaling(bool scaling) { autoScaling = scaling; }
	bool hasImageAutoScaling() const { return autoScaling; }
	void setZoomFactor(float factor);
	float getZoomFactor() const { return zoomFactor; }
	QPoint mapScaledImagePositionToOriginalPixel(const QPoint& pos);

protected:
	virtual void mouseMoveEvent(QMouseEvent* evt);
	virtual void resizeEvent(QResizeEvent* evt);

private:
	void updateScaledPixmap();

signals:
	void mouseCursorMoved(int x, int y, const QColor& origPixel, const QColor& scaledPixel);

private:
	bool autoScaling;
	float zoomFactor;
	QPixmap basePixmap;
	QPixmap scaledPixmap;
	QImage baseImage;
	QImage scaledImage;
	QTransform scaleTransform;
	QTransform invScaleTransform;
};

#endif /* IMAGEDISPLAYLABEL_H_ */
