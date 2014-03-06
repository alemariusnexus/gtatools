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
