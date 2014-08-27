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

#include "IFPAnimationViewerWidget.h"
#include "../../ProfileManager.h"
#include <gtaformats/dff/DFFLoader.h>
#include <gtaformats/ifp/IFPLoader.h>
#include <nxcommon/file/DefaultFileFinder.h>
#include <gta/resource/animation/AnimationPackage.h>
#include <gta/resource/animation/AnimationCacheEntry.h>
#include <gta/resource/animation/StaticAnimationPackagePointer.h>
#include <gta/resource/mesh/StaticMeshPointer.h>
#include <gta/resource/mesh/ManagedMeshPointer.h>
#include <gta/resource/texture/StaticTextureSource.h>
#include <gta/resource/texture/ManagedTextureSource.h>
#include <QFileDialog>



IFPAnimationViewerWidget::IFPAnimationViewerWidget(QWidget* parent)
		: QWidget(parent), frameModel(NULL), anpk(NULL), currentAnim(NULL), displayedFrame(NULL)
{
	ui.setupUi(this);

	ui.animTimeBeginButton->setIcon(QIcon::fromTheme("media-skip-backward"));
	ui.animTimeEndButton->setIcon(QIcon::fromTheme("media-skip-forward"));
	ui.animTimeBackwardButton->setIcon(QIcon::fromTheme("media-seek-backward"));
	ui.animTimeForwardButton->setIcon(QIcon::fromTheme("media-seek-forward"));
	ui.animTimePlayButton->setIcon(QIcon::fromTheme("media-playback-start"));

	boneSizeSpinner = new QDoubleSpinBox(this);
	boneSizeSpinner->setMinimum(0.0);
	boneSizeSpinner->setSingleStep(0.01f);
	boneSizeSpinner->setDecimals(5);

	QLabel* boneSizeLabel = new QLabel(tr("Bone Length:"), this);

	boneRenderingAction = new QAction(tr("Bone Rendering"), this);
	boneRenderingAction->setCheckable(true);
	connect(boneRenderingAction, SIGNAL(toggled(bool)), this, SLOT(boneRenderingToggled(bool)));

	ui.animSlider->setRange(0, 10000);

	connect(ui.modelChooseButton, SIGNAL(pressed()), this, SLOT(modelFileChosen()));
	connect(ui.animChooseButton, SIGNAL(pressed()), this, SLOT(animFileChosen()));
	connect(ui.txdChooseButton, SIGNAL(pressed()), this, SLOT(txdFileChosen()));

	connect(ui.displayButton, SIGNAL(pressed()), this, SLOT(displayRequested()));

	connect(ui.animList, SIGNAL(itemActivated(QListWidgetItem*)), this,
			SLOT(animationActivated(QListWidgetItem*)));

	connect(ui.animSlider, SIGNAL(valueChanged(int)), this, SLOT(animTimeChanged(int)));

	connect(ui.animTimeBeginButton, SIGNAL(pressed()), this, SLOT(animTimeBeginRequested()));
	connect(ui.animTimeBackwardButton, SIGNAL(pressed()), this, SLOT(animTimeBackwardRequested()));
	connect(ui.animTimePlayButton, SIGNAL(pressed()), this, SLOT(animTimePlayRequested()));
	connect(ui.animTimeForwardButton, SIGNAL(pressed()), this, SLOT(animTimeForwardRequested()));
	connect(ui.animTimeEndButton, SIGNAL(pressed()), this, SLOT(animTimeEndRequested()));

	connect(&animPlayTimer, SIGNAL(timeout()), this, SLOT(playAnimation()));

	connect(ui.frameView, SIGNAL(activated(const QModelIndex&)), this,
			SLOT(frameActivated(const QModelIndex&)));

	animPlayTimer.setInterval(10);

	renderContainerWidget = new GLContainerWidget(ui.renderContainerWidget);
	ui.renderContainerWidget->layout()->addWidget(renderContainerWidget);
	renderWidget = new IFPAnimationViewerRenderWidget(renderContainerWidget);
	renderContainerWidget->setGLWidget(renderWidget);

	boneSizeSpinner->setValue(renderWidget->getBoneLength());

	connect(boneSizeSpinner, SIGNAL(valueChanged(double)), this, SLOT(boneSizeChanged(double)));

	QToolBar* contBar = renderContainerWidget->getToolBar();

	contBar->addSeparator();
	contBar->addAction(boneRenderingAction);

	contBar->addSeparator();
	contBar->addWidget(boneSizeLabel);
	contBar->addWidget(boneSizeSpinner);
}


void IFPAnimationViewerWidget::reloadModelFile()
{
	QString fname = ui.modelField->text();
	File file(fname.toLocal8Bit().constData());

	MeshPointer* meshPtr;

	if (file.exists()) {
		DFFLoader dff;
		DFFMesh* dffMesh = dff.loadMesh(file);

		MeshClump* clump = new MeshClump(dffMesh);

		delete dffMesh;

		meshPtr = new StaticMeshPointer(clump);
	} else {
		meshPtr = new ManagedMeshPointer(fname.toLocal8Bit().constData());
	}

	renderWidget->setMeshPointer(meshPtr);

	if (frameModel)
		delete frameModel;

	MeshClump* clump = **meshPtr;
	frameModel = new IFPFrameItemModel(clump);

	ui.frameView->setModel(frameModel);
}


void IFPAnimationViewerWidget::reloadAnimationFile()
{
	if (currentAnim) {
		currentAnim = NULL;
	}
	if (anpk) {
		delete anpk;
		anpk = NULL;
	}

	ui.animSlider->setEnabled(false);

	ui.animList->clear();

	QString fname = ui.animField->text();
	File file(fname.toLocal8Bit().constData());

	if (!file.exists()) {
		Engine::StringResourceCache::Entry* entry = Engine::getInstance()->getAnimationCache()
				->getEntryPointer(fname.toLocal8Bit().constData()).getEntry();

		if (!entry)
			return;

		anpk = ((AnimationCacheEntry*) entry)->getPackage();
	} else {
		anpk = new AnimationPackage;

		IFPLoader ifp(file);

		IFPAnimation* ianim;
		while ((ianim = ifp.readAnimation())  !=  NULL) {
			Animation* anim = new Animation(ianim);
			anpk->addAnimation(CString(ianim->getName()).lower(), anim);
			delete ianim;
		}
	}

	for (AnimationPackage::AnimIterator it = anpk->getAnimationBegin() ; it != anpk->getAnimationEnd() ; it++) {
		Animation* anim = it->second;
		CString animName = it->first;
		ui.animList->addItem(animName.get());
	}

	StaticAnimationPackagePointer* anpkPtr = new StaticAnimationPackagePointer(anpk);
	renderWidget->setAnimationPackagePointer(anpkPtr);
}


void IFPAnimationViewerWidget::reloadTXDFile()
{
	QString fname = ui.txdField->text();
	File file(fname.toLocal8Bit().constData());

	if (file.exists()) {
		TXDArchive txd(file);
		StaticTextureSource* texSrc = new StaticTextureSource(&txd);
		renderWidget->setTextureSource(texSrc);
	} else {
		ManagedTextureSource* texSrc = new ManagedTextureSource(fname.toLocal8Bit().constData());
		renderWidget->setTextureSource(texSrc);
	}
}


void IFPAnimationViewerWidget::modelFileChosen()
{
	QString fname = QFileDialog::getOpenFileName(this, tr("Choose model file"),
			QString(), tr("Model Files (*.dff)"));

	if (!fname.isNull()) {
		ui.modelField->setText(fname);
	}
}


void IFPAnimationViewerWidget::animFileChosen()
{
	QString fname = QFileDialog::getOpenFileName(this, tr("Choose animation file"),
			QString(), tr("Animation Files (*.ifp)"));

	if (!fname.isNull()) {
		ui.animField->setText(fname);
		reloadAnimationFile();
	}
}


void IFPAnimationViewerWidget::txdFileChosen()
{
	QString fname = QFileDialog::getOpenFileName(this, tr("Choose TXD file"),
			QString(), tr("TXD Files (*.txd)"));

	if (!fname.isNull()) {
		ui.txdField->setText(fname);
	}
}


void IFPAnimationViewerWidget::displayRequested()
{
	reloadModelFile();
	reloadAnimationFile();
	reloadTXDFile();

	renderWidget->updateDisplay();
}


void IFPAnimationViewerWidget::setCurrentAnimation(const CString& animName)
{
	Animation* anim = anpk->find(animName);
	currentAnim = anim;
	ui.animSlider->setEnabled(true);

	renderWidget->setCurrentAnimation(animName);

	setAnimationTime(0.0f);
}


void IFPAnimationViewerWidget::setAnimationTime(float time)
{
	currentTime = time;

	int value = (time / currentAnim->getDuration()) * 10000;

	if (ui.animSlider->value() != value) {
		ui.animSlider->setValue(value);
	}

	ui.animTimeLabel->setText(QString("%1 / %2").arg(time, 0, 'f', 7)
			.arg(currentAnim->getDuration(), 0, 'f', 7));

	renderWidget->setAnimationTime(time);

	updateFrameDisplay();
}


void IFPAnimationViewerWidget::animationActivated(QListWidgetItem* item)
{
	setCurrentAnimation(item->text().toLocal8Bit().constData());
}


void IFPAnimationViewerWidget::animTimeChanged(int value)
{
	float time = currentAnim->getDuration() * (value / 10000.0f);
	setAnimationTime(time);
}


void IFPAnimationViewerWidget::animTimeBeginRequested()
{
	setAnimationTime(0.0f);
}


void IFPAnimationViewerWidget::animTimeBackwardRequested()
{
	float newTime = currentTime - (currentAnim->getDuration() / 100.0f);

	if (newTime < 0.0f)
		newTime = 0.0f;

	setAnimationTime(newTime);
}


void IFPAnimationViewerWidget::animTimePlayRequested()
{
	if (animPlayTimer.isActive()) {
		animPlayTimer.stop();
		ui.animTimePlayButton->setIcon(QIcon::fromTheme("media-playback-start"));
	} else {
		lastPlayTime = GetTickcount();
		animPlayTimer.start();
		ui.animTimePlayButton->setIcon(QIcon::fromTheme("media-playback-pause"));
	}
}


void IFPAnimationViewerWidget::animTimeForwardRequested()
{
	float newTime = currentTime + (currentAnim->getDuration() / 100.0f);

	if (newTime > currentAnim->getDuration())
		newTime = currentAnim->getDuration();

	setAnimationTime(newTime);
}


void IFPAnimationViewerWidget::animTimeEndRequested()
{
	setAnimationTime(currentAnim->getDuration());
}


void IFPAnimationViewerWidget::playAnimation()
{
	uint64_t time = GetTickcount();

	float newTime = fmod(currentTime + (time - lastPlayTime) / 1000.0f, currentAnim->getDuration());
	setAnimationTime(newTime);

	lastPlayTime = time;
}


void IFPAnimationViewerWidget::boneRenderingToggled(bool br)
{
	renderWidget->setBoneRendering(br);
	renderWidget->updateDisplay();
}


void IFPAnimationViewerWidget::boneSizeChanged(double s)
{
	renderWidget->setBoneLength((float) s);
	renderWidget->updateDisplay();
}


void IFPAnimationViewerWidget::frameActivated(const QModelIndex& idx)
{
	if (!idx.isValid())
		return;

	MeshFrame* frame = (MeshFrame*) idx.internalPointer();

	setDisplayedFrame(frame);
}


void IFPAnimationViewerWidget::updateFrameDisplay()
{
	if (!displayedFrame)
		return;

	AnimationBone* bone = NULL;

	if (currentAnim)
		bone = currentAnim->getBoneForFrame(displayedFrame);

	renderWidget->setCurrentBone(displayedFrame->getName());
	renderWidget->updateDisplay();

	if (bone) {
		AnimationFrame* f1;
		AnimationFrame* f2;
		float t;

		bone->getFrames(currentTime, f1, f2, t);

		AnimationFrame* f = bone->getInterpolatedFrame(currentTime);

		ui.ipFactorLabel->setText(QString("%1").arg(t));

		Quaternion f1r = f1->getRotation();
		Vector3 f1t = f1->getTranslation();
		Vector3 f1s = f1->getScale();
		ui.f1RotationLabel->setText(tr("(%1, %2, %3, %4)")
				.arg(f1r.getW()).arg(f1r.getX()).arg(f1r.getY()).arg(f1r.getZ()));
		ui.f1TranslationLabel->setText(tr("(%1, %2, %3)")
				.arg(f1t.getX()).arg(f1t.getY()).arg(f1t.getZ()));
		ui.f1ScaleLabel->setText(tr("(%1, %2, %3)")
				.arg(f1s.getX()).arg(f1s.getY()).arg(f1s.getZ()));
		ui.f1TimeLabel->setText(QString("%1").arg(f1->getStart()));

		Quaternion f2r = f2->getRotation();
		Vector3 f2t = f2->getTranslation();
		Vector3 f2s = f2->getScale();
		ui.f2RotationLabel->setText(tr("(%1, %2, %3, %4)")
				.arg(f2r.getW()).arg(f2r.getX()).arg(f2r.getY()).arg(f2r.getZ()));
		ui.f2TranslationLabel->setText(tr("(%1, %2, %3)")
				.arg(f2t.getX()).arg(f2t.getY()).arg(f2t.getZ()));
		ui.f2ScaleLabel->setText(tr("(%1, %2, %3)")
				.arg(f2s.getX()).arg(f2s.getY()).arg(f2s.getZ()));
		ui.f2TimeLabel->setText(QString("%1").arg(f2->getStart()));

		Quaternion ifr = f->getRotation();
		Vector3 ift = f->getTranslation();
		Vector3 ifs = f->getScale();
		ui.ipRotationLabel->setText(tr("(%1, %2, %3, %4)")
				.arg(ifr.getW()).arg(ifr.getX()).arg(ifr.getY()).arg(ifr.getZ()));
		ui.ipTranslationLabel->setText(tr("(%1, %2, %3)")
				.arg(ift.getX()).arg(ift.getY()).arg(ift.getZ()));
		ui.ipScaleLabel->setText(tr("(%1, %2, %3)")
				.arg(ifs.getX()).arg(ifs.getY()).arg(ifs.getZ()));
		ui.ipTimeLabel->setText(QString("%1").arg(f->getStart()));
	} else {
		ui.ipFactorLabel->setText(tr("-"));

		ui.f1RotationLabel->setText(tr("-"));
		ui.f1TranslationLabel->setText(tr("-"));
		ui.f1ScaleLabel->setText(tr("-"));
		ui.f1TimeLabel->setText(tr("-"));

		ui.f2RotationLabel->setText(tr("-"));
		ui.f2TranslationLabel->setText(tr("-"));
		ui.f2ScaleLabel->setText(tr("-"));
		ui.f2TimeLabel->setText(tr("-"));

		ui.ipRotationLabel->setText(tr("-"));
		ui.ipTranslationLabel->setText(tr("-"));
		ui.ipScaleLabel->setText(tr("-"));
		ui.ipTimeLabel->setText(tr("-"));
	}
}


void IFPAnimationViewerWidget::setDisplayedFrame(MeshFrame* frame)
{
	displayedFrame = frame;
	updateFrameDisplay();
}
