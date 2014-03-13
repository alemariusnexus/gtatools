/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#include "ImageDisplayLabel.h"
#include <QtGui/QImage>
#include <QtGui/QBitmap>
#include "TXDFormatHandler.h"
#include "../../System.h"



ImageDisplayLabel::ImageDisplayLabel(const QPixmap& pixmap, QWidget* parent)
		: QLabel(parent), autoScaling(false), zoomFactor(1.0f)
{
	display(pixmap);
	setMouseTracking(true);
}


ImageDisplayLabel::ImageDisplayLabel(QWidget* parent)
		: QLabel(parent), autoScaling(false), zoomFactor(1.0f)
{
	setMouseTracking(true);
}


ImageDisplayLabel::~ImageDisplayLabel()
{
}


void ImageDisplayLabel::display(const QPixmap& pixmap)
{
	basePixmap = pixmap;
	baseImage = pixmap.toImage();
	updateScaledPixmap();
}


void ImageDisplayLabel::setZoomFactor(float factor)
{
	zoomFactor = factor;
	updateScaledPixmap();
}


QPoint ImageDisplayLabel::mapScaledImagePositionToOriginalPixel(const QPoint& pos)
{
	QPointF fPos(pos.x(), pos.y());
	QPointF umPos = invScaleTransform.map(fPos);
	return QPoint((int) floor(umPos.x()), (int) floor(umPos.y()));
}


void ImageDisplayLabel::updateScaledPixmap()
{
	if (basePixmap.isNull())
		return;

	float zf = zoomFactor;

	if (autoScaling) {
		zf = qMin(width() / (float) basePixmap.width(), height() / (float) basePixmap.height());
	}

	scaleTransform = QTransform::fromScale(zf, zf);
	invScaleTransform = scaleTransform.inverted();

	scaledPixmap = basePixmap.transformed(scaleTransform);
	scaledImage = scaledPixmap.toImage();

	setPixmap(scaledPixmap);
}


void ImageDisplayLabel::mouseMoveEvent(QMouseEvent* evt)
{
	const QPoint pos = evt->pos();

	QPoint opos = mapScaledImagePositionToOriginalPixel(pos);

	if (opos.x() >= 0  &&  opos.y() >= 0  &&  opos.x() < basePixmap.width()  &&  opos.y() < basePixmap.height()) {
		QColor scaledPixel(scaledImage.pixel(pos.x(), pos.y()));
		QColor origPixel(baseImage.pixel(opos.x(), opos.y()));

		emit mouseCursorMoved(opos.x(), opos.y(), origPixel, scaledPixel);
	}
}


void ImageDisplayLabel::resizeEvent(QResizeEvent* evt)
{
	updateScaledPixmap();
}
