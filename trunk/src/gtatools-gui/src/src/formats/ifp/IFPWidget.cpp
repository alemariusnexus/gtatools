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

#include "IFPWidget.h"
#include <gtaformats/ifp/IFPLoader.h>
#include <QtGui/QListWidgetItem>
#include <algorithm>

using std::advance;



IFPWidget::IFPWidget(const File& file)
{
	ui.setupUi(this);

	IFPLoader ifp(file);

	unsigned int i = 0;
	IFPAnimation* anim;
	while ((anim = ifp.readAnimation())  !=  NULL) {
		anims << anim;
		QListWidgetItem* item = new QListWidgetItem(anim->getName().get());
		ui.animList->addItem(item);
		i++;
	}

	connect(ui.animList, SIGNAL(currentRowChanged(int)), this, SLOT(currentAnimChanged(int)));
	connect(ui.objList, SIGNAL(currentRowChanged(int)), this, SLOT(currentObjectChanged(int)));
	connect(ui.frameNumSpinner, SIGNAL(valueChanged(int)), this, SLOT(frameNumSpinnerChanged(int)));
	connect(ui.frameNumSlider, SIGNAL(sliderMoved(int)), this, SLOT(frameNumSliderChanged(int)));
}


void IFPWidget::currentAnimChanged(int row)
{
	if (row == -1) {
		ui.objList->clear();
		return;
	}

	IFPAnimation* anim = anims[row];

	ui.objList->clear();

	for (IFPAnimation::ObjectIterator it = anim->getObjectBegin() ; it != anim->getObjectEnd() ; it++) {
		IFPObject* obj = *it;

		ui.objList->addItem(!obj->getName().isNull() ? obj->getName().get() : "[UNNAMED]");
	}
}


void IFPWidget::currentObjectChanged(int row)
{
	if (row == -1) {
		ui.frameTypeLabel->setText(tr("-"));
		ui.boneIDLabel->setText(tr("-"));
		return;
	}

	IFPAnimation* anim = anims[ui.animList->currentRow()];
	IFPAnimation::ObjectIterator it = anim->getObjectBegin();
	advance(it, row);
	IFPObject* obj = *it;

	switch (obj->getFrameType()) {
	case IFPObject::RotFrame:
		ui.frameTypeLabel->setText(tr("Rotation Frames (Child Frames)"));
		break;
	case IFPObject::RotTransFrame:
		ui.frameTypeLabel->setText(tr("Rotation/Translation Frames (Root Frames)"));
		break;
	case IFPObject::RotTransScaleFrame:
		ui.frameTypeLabel->setText(tr("Rotation/Translation/Scale Frames"));
		break;
	case IFPObject::Unknown2Frame:
		ui.frameTypeLabel->setText(tr("Unknown (FrameType = 2)"));
		break;
	}

	ui.boneIDLabel->setText(QString("%1").arg(obj->getBoneID()));

	printf("Object has %d frames\n", obj->getFrameCount());
	ui.frameNumSpinner->setRange(0, obj->getFrameCount()-1);
	ui.frameNumSlider->setRange(0, obj->getFrameCount()-1);
	ui.frameNumSpinner->setValue(0);
	ui.frameNumSlider->setValue(0);
	displayFrame(0);
}


void IFPWidget::frameNumSpinnerChanged(int frame)
{
	ui.frameNumSlider->setValue(frame);
	displayFrame(frame);
}


void IFPWidget::frameNumSliderChanged(int frame)
{
	ui.frameNumSpinner->setValue(frame);
	displayFrame(frame);
}


void IFPWidget::displayFrame(int fnum)
{
	IFPAnimation* anim = anims[ui.animList->currentRow()];

	IFPAnimation::ObjectIterator it = anim->getObjectBegin();
	advance(it, ui.objList->currentRow());
	IFPObject* obj = *it;

	IFPObject::FrameIterator fit = obj->getFrameBegin();
	advance(fit, fnum);
	IFPRotFrame* frame = *fit;

	Quaternion rot = frame->getRotation();
	ui.frameRotLabel->setText(tr("%1, %2, %3, %4").arg(rot.getX()).arg(rot.getY()).arg(rot.getZ())
			.arg(rot.getW()));

	ui.frameTimeLabel->setText(QString("%1").arg(frame->getTime()));

	if (obj->getFrameType() == IFPObject::RotTransFrame) {
		IFPRotTransFrame* rtframe = (IFPRotTransFrame*) frame;
		Vector3 trans = rtframe->getTranslation();

		ui.frameTransLabel->setText(tr("%1, %2, %3").arg(trans.getX()).arg(trans.getY()).arg(trans.getZ()));
		ui.frameScaleLabel->setText(tr("-"));
	} else if (obj->getFrameType() == IFPObject::RotTransScaleFrame) {
		IFPRotTransScaleFrame* rtsframe = (IFPRotTransScaleFrame*) frame;
		Vector3 trans = rtsframe->getTranslation();
		Vector3 scale = rtsframe->getScale();

		ui.frameTransLabel->setText(tr("%1, %2, %3").arg(trans.getX()).arg(trans.getY()).arg(trans.getZ()));
		ui.frameScaleLabel->setText(tr("%1, %2, %3").arg(scale.getX()).arg(scale.getY()).arg(scale.getZ()));
	} else {
		ui.frameTransLabel->setText(tr("-"));
		ui.frameScaleLabel->setText(tr("-"));
	}
}
