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

#ifndef IMAGEDISPLAYWIDGET_H_
#define IMAGEDISPLAYWIDGET_H_

#include <QtGui/QWidget>
#include <QtGui/QScrollArea>
#include <QtGui/QToolBar>
#include <QtGui/QToolButton>
#include <QtGui/QSlider>
#include <QtGui/QImage>
#include <QtGui/QPixmap>
#include <QtGui/QAction>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include "ImageDisplayLabel.h"
#include "PercentageValidator.h"


class ImageDisplayWidget : public QWidget
{
	Q_OBJECT

public:
	ImageDisplayWidget(QWidget* parent = NULL);
	void display(const QImage& image);
	void setZoomFactorRange(float min, float max);
	void setZoomFactor(float factor);
	float getZoomFactor() const { return displayLabel->getZoomFactor(); }

private:
	int zoomFactorToSliderValue(float factor);

private slots:
	void zoomSliderChanged(int value);
	void zoomSliderReleased();
	void zoomBoxTextChanged();
	void zoomOrigRequested();
	void zoomOutRequested();
	void zoomInRequested();
	void currentPixelChanged(int x, int y, const QColor& origPixel, const QColor& scaledPixel);

private:
	QScrollArea* scroller;
	ImageDisplayLabel* displayLabel;
	QToolBar* toolBar;
	QSlider* zoomSlider;
	QComboBox* zoomBox;
	PercentageValidator* zoomBoxValidator;
	QLabel* pixelDisplayLabel;

	QAction* zoomOrigAction;
	QAction* zoomOutAction;
	QAction* zoomInAction;

	QImage image;
	QPixmap pixmap;
	float zoomMin;
	float zoomMax;
};

#endif /* IMAGEDISPLAYWIDGET_H_ */
