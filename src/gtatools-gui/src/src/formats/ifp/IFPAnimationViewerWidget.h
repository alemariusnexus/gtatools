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

#ifndef IFPANIMATIONVIEWERWIDGET_H_
#define IFPANIMATIONVIEWERWIDGET_H_

#include <ui_IFPAnimationViewerWidget.h>
#include <gta/Animation.h>
#include <gta/resource/animation/AnimationPackage.h>
#include <QWidget>
#include <QDoubleSpinBox>
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
