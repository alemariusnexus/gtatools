/*
 * IFPAnimationViewerWidget.h
 *
 *  Created on: 27.05.2012
 *      Author: alemariusnexus
 */

#ifndef IFPANIMATIONVIEWERWIDGET_H_
#define IFPANIMATIONVIEWERWIDGET_H_

#include <ui_IFPAnimationViewerWidget.h>
#include <gta/Animation.h>
#include <gta/resource/animation/AnimationPackage.h>
#include <QtGui/QWidget>
#include <QtGui/QDoubleSpinBox>
#include <QtCore/QTimer>
#include "IFPAnimationViewerRenderWidget.h"
#include "IFPFrameItemModel.h"
#include "../../gui/GLContainerWidget.h"



class IFPAnimationViewerWidget : public QWidget
{
	Q_OBJECT

public:
	IFPAnimationViewerWidget(QWidget* parent = NULL);

private:
	void reloadModelFile();
	void reloadAnimationFile();
	void reloadTXDFile();
	void setCurrentAnimation(const CString& animName);
	void setAnimationTime(float time);
	void setDisplayedFrame(MeshFrame* frame);
	void updateFrameDisplay();

private slots:
	void modelFileChosen();
	void animFileChosen();
	void txdFileChosen();
	void displayRequested();
	void animationActivated(QListWidgetItem* item);
	void animTimeChanged(int value);
	void animTimeBeginRequested();
	void animTimeBackwardRequested();
	void animTimePlayRequested();
	void animTimeForwardRequested();
	void animTimeEndRequested();
	void playAnimation();
	void boneRenderingToggled(bool br);
	void boneSizeChanged(double s);
	void frameActivated(const QModelIndex& idx);

private:
	Ui_IFPAnimationViewerWidget ui;

	QAction* boneRenderingAction;
	QDoubleSpinBox* boneSizeSpinner;

	IFPAnimationViewerRenderWidget* renderWidget;
	GLContainerWidget* renderContainerWidget;
	QTimer animPlayTimer;
	IFPFrameItemModel* frameModel;

	uint64_t lastPlayTime;
	AnimationPackage* anpk;
	Animation* currentAnim;
	float currentTime;
	MeshFrame* displayedFrame;
};

#endif /* IFPANIMATIONVIEWERWIDGET_H_ */
