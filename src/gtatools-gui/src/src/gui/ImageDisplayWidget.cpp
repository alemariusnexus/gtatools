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

#include "ImageDisplayWidget.h"
#include <cmath>
#include <QVBoxLayout>
#include <QLineEdit>
#include "PercentageValidator.h"




ImageDisplayWidget::ImageDisplayWidget(QWidget* parent)
		: QWidget(parent)
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	setLayout(mainLayout);

	scroller = new QScrollArea(this);
	scroller->setWidgetResizable(true);
	mainLayout->addWidget(scroller, 1);

	displayLabel = new ImageDisplayLabel(scroller);
	displayLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	scroller->setWidget(displayLabel);

	toolBar = new QToolBar(this);
	mainLayout->addWidget(toolBar);

	zoomBoxValidator = new PercentageValidator(this);

	zoomBox = new QComboBox(toolBar);
	zoomBox->setEditable(true);
	zoomBox->setInsertPolicy(QComboBox::NoInsert);
	zoomBox->setValidator(zoomBoxValidator);

	connect(zoomBox->lineEdit(), SIGNAL(editingFinished()), this, SLOT(zoomBoxTextChanged()));
	connect(zoomBox, SIGNAL(activated(const QString&)), this, SLOT(zoomBoxTextChanged()));

	zoomSlider = new QSlider(Qt::Horizontal, toolBar);
	connect(zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(zoomSliderChanged(int)));
	connect(zoomSlider, SIGNAL(sliderReleased()), this, SLOT(zoomSliderReleased()));
	zoomSlider->setMinimumWidth(250);
	zoomSlider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

	setZoomFactorRange(0.25f, 16.0f);
	setZoomFactor(1.0f);

	zoomSlider->setValue(0);

	pixelDisplayLabel = new QLabel(toolBar);
	QFont font("Monospace");
	font.setStyleHint(QFont::Monospace);
	pixelDisplayLabel->setFont(font);

	connect(displayLabel, SIGNAL(mouseCursorMoved(int, int, const QColor&, const QColor&)), this,
			SLOT(currentPixelChanged(int, int, const QColor&, const QColor&)));

	zoomOrigAction = new QAction(QIcon::fromTheme("zoom-original"), "", toolBar);
	zoomOrigAction->setShortcut(QKeySequence("SPACE"));
	zoomOutAction = new QAction(QIcon::fromTheme("zoom-out"), "", toolBar);
	zoomOutAction->setShortcut(QKeySequence("-"));
	zoomInAction = new QAction(QIcon::fromTheme("zoom-in"), "", toolBar);
	zoomInAction->setShortcut(QKeySequence("+"));

	addAction(zoomOrigAction);
	addAction(zoomOutAction);
	addAction(zoomInAction);

	connect(zoomOrigAction, SIGNAL(triggered()), this, SLOT(zoomOrigRequested()));
	connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOutRequested()));
	connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomInRequested()));

	toolBar->addWidget(zoomBox);
	toolBar->addAction(zoomOrigAction);
	//toolBar->addSeparator();
	toolBar->addAction(zoomOutAction);
	toolBar->addWidget(zoomSlider);
	toolBar->addAction(zoomInAction);

	QWidget* spacer = new QWidget(toolBar);
	spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	toolBar->addWidget(spacer);

	toolBar->addWidget(pixelDisplayLabel);
}


void ImageDisplayWidget::display(const QImage& image)
{
	this->image = image;
	pixmap = QPixmap::fromImage(image);

	displayLabel->display(pixmap);
}


void ImageDisplayWidget::setZoomFactorRange(float min, float max)
{
	zoomMin = min;
	zoomMax = max;

	zoomBox->clear();

	QStringList zoomBoxEntries;

	float factor, lastFactor;
	lastFactor = min;

	for (factor = zoomMin ; factor < zoomMax-0.1f ; factor *= 2.0f) {
		if (lastFactor < 1.0f  &&  factor > 1.0f  &&  fabs(factor-lastFactor) > 0.1f) {
			zoomBoxEntries << "100%";
		}

		zoomBoxEntries << QString("%1%").arg(factor * 100.0f, 0, 'f', 1);
		lastFactor = factor;
	}

	if (fabs(lastFactor-zoomMax) > 0.1f) {
		zoomBoxEntries << QString("%1%").arg(zoomMax * 100.0f, 0, 'f', 1);
	}

	for (int i = zoomBoxEntries.size()-1 ; i >= 0 ; i--) {
		zoomBox->addItem(zoomBoxEntries[i]);
	}

	zoomBoxValidator->setRange(zoomMin*100.0f, zoomMax*100.0f, 1);

	zoomSlider->setRange(zoomFactorToSliderValue(zoomMin), zoomFactorToSliderValue(zoomMax));
}


void ImageDisplayWidget::setZoomFactor(float factor)
{
	factor = qMax(qMin(factor, zoomMax), zoomMin);

	zoomBox->blockSignals(true);
	zoomSlider->blockSignals(true);

	zoomBox->setEditText(QString("%1%").arg(factor * 100.0f, 0, 'f', 1));
	zoomSlider->setValue(zoomFactorToSliderValue(factor));

	zoomSlider->blockSignals(false);
	zoomBox->blockSignals(false);

	displayLabel->setZoomFactor(factor);
}


int ImageDisplayWidget::zoomFactorToSliderValue(float factor)
{
	float v = factor - 1.0f;
	return v >= 0 ? floor(v*100.0f + 0.5f) : floor(100.0f - 100.0f/factor + 0.5f);
}


void ImageDisplayWidget::zoomSliderChanged(int value)
{
	float factor = value >= 0 ? (1.0f + value / 100.0f) : (1.0f / (1.0f - (value / 100.0f)));

	setZoomFactor(factor);
}


void ImageDisplayWidget::zoomSliderReleased()
{
}


void ImageDisplayWidget::zoomBoxTextChanged()
{
	QString text = zoomBox->currentText().trimmed();

	if (text.endsWith('%')) {
		text = text.left(text.length()-1);
	}

	bool ok;
	float percentage = text.toFloat(&ok);

	if (!ok)
		return;

	float factor = percentage/100.0f;

	setZoomFactor(factor);
}


void ImageDisplayWidget::zoomOrigRequested()
{
	setZoomFactor(1.0f);
}


void ImageDisplayWidget::zoomOutRequested()
{
	setZoomFactor(getZoomFactor() * 0.5f);
}


void ImageDisplayWidget::zoomInRequested()
{
	setZoomFactor(getZoomFactor() * 2.0f);
}


void ImageDisplayWidget::currentPixelChanged(int x, int y, const QColor& origPixel, const QColor& scaledPixel)
{
	pixelDisplayLabel->setText(QString("(%1, %2) RGBA: (%3, %4, %5, %6)").arg(x, 4).arg(y, 4)
			.arg(origPixel.red(), 3).arg(origPixel.green(), 3).arg(origPixel.blue(), 3).arg(origPixel.alpha(), 3));
}

