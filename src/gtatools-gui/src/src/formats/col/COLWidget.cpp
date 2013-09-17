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


#include "COLWidget.h"
#include "COLGUIModule.h"
#include "../../System.h"
#include <QtCore/QSettings>



COLWidget::COLWidget(const File& file, QWidget *parent)
		: QWidget(parent), currentlyCompact(false)
{
	ui.setupUi(this);

	sphereBoxTabber = new QTabWidget;
	sphereBoxTabber->setParent(ui.sphereBoxInfoWidget);
	ui.sphereBoxInfoWidgetLayout->addWidget(sphereBoxTabber);

	vmeshTabber = new QTabWidget;
	vmeshTabber->setParent(ui.vmeshInfoWidget);
	ui.vmeshInfoWidgetLayout->addWidget(vmeshTabber);

	smeshTabber = new QTabWidget;
	smeshTabber->setParent(ui.smeshTab);
	ui.smeshTabLayout->addWidget(smeshTabber);

	/*sphereBoxRendererContainer = new GLContainerWidget(ui.sphereBoxRenderWidgetContainer);
	ui.sphereBoxRenderWidgetContainer->layout()->addWidget(sphereBoxRendererContainer);
	sphereBoxRenderer = new COLSphereBoxRenderWidget(sphereBoxRendererContainer);
	sphereBoxRendererContainer->setGLWidget(sphereBoxRenderer);

	shadowMeshRendererContainer = new GLContainerWidget(ui.smeshRenderWidgetContainer);
	ui.smeshRenderWidgetContainer->layout()->addWidget(shadowMeshRendererContainer);
	shadowMeshRenderer = new COLMeshRenderWidget(shadowMeshRendererContainer);
	shadowMeshRendererContainer->setGLWidget(shadowMeshRenderer);*/

	meshRendererContainer = new GLContainerWidget(ui.vmeshRenderWidgetContainer);
	ui.vmeshRenderWidgetContainer->layout()->addWidget(meshRendererContainer);
	meshRenderer = new COLMeshRenderWidget(meshRendererContainer);
	meshRendererContainer->setGLWidget(meshRenderer);

	COLLoader col;
	istream* stream = file.openInputStream(istream::binary);

	COLModel* model;

	while ((model = col.loadModel(stream))) {
		models << model;
		ui.modelList->addItem(model->getName());
	}

	System* sys = System::getInstance();

	sys->installGUIModule(COLGUIModule::getInstance());

	connect(ui.modelList, SIGNAL(currentRowChanged(int)), this, SLOT(currentModelChanged(int)));
	connect(ui.faceGroupBox, SIGNAL(toggled(bool)), this, SLOT(faceGroupsToggled(bool)));
	connect(ui.faceGroupList, SIGNAL(itemSelectionChanged()), this, SLOT(faceGroupSelectionChanged()));
	connect(ui.meshList, SIGNAL(currentRowChanged(int)), this, SLOT(currentMeshChanged(int)));
	/*connect(shadowMeshRenderer, SIGNAL(faceSelectionChanged(int, int)), this,
			SLOT(shadowMeshFaceSelectionChanged(int, int)));*/
	connect(meshRenderer, SIGNAL(faceSelectionChanged(int, int)), this,
			SLOT(vertexMeshFaceSelectionChanged(int, int)));
	connect(sys, SIGNAL(configurationChanged()), this, SLOT(updateLayoutType()));

	ui.faceGroupBox->setChecked(false);

	updateLayoutType();
}


COLWidget::~COLWidget()
{
	QList<COLModel*>::iterator it;

	for (it = models.begin() ; it != models.end() ; it++) {
		delete *it;
	}

	COLGUIModule::getInstance()->disconnect(this);
	System::getInstance()->uninstallGUIModule(COLGUIModule::getInstance());
}


void COLWidget::updateLayoutType()
{
	QSettings settings;

	bool compact = settings.value("gui/compact_mode", false).toBool();

	if (compact  &&  !currentlyCompact) {
		// Spheres and Boxes
		ui.sphereBoxGeneralInfoWidget->setParent(sphereBoxTabber);
		sphereBoxTabber->addTab(ui.sphereBoxGeneralInfoWidget, tr("General Information"));

		ui.sphereBoxRenderWidgetContainer->setParent(sphereBoxTabber);
		sphereBoxTabber->addTab(ui.sphereBoxRenderWidgetContainer, tr("Rendering"));

		ui.sphereBoxRenderWidgetContainerLayout->setContentsMargins(4, 4, 4, 4);
		ui.sphereBoxGeneralInfoWidgetLayout->setContentsMargins(4, 4, 4, 4);

		ui.sphereBoxRenderWidgetContainer->show();
		ui.sphereBoxGeneralInfoWidget->show();

		ui.sphereBoxGeneralInfoGroupBox->hide();
		ui.sphereBoxRenderGroupBox->hide();

		// TODO: This is a hack. Without it, some graphical problems occur on the sphereBoxTabber.
		sphereBoxTabber->setCurrentWidget(ui.sphereBoxRenderWidgetContainer);
		sphereBoxTabber->setCurrentWidget(ui.sphereBoxGeneralInfoWidget);

		sphereBoxTabber->show();


		// Vertex Mesh
		ui.vmeshRenderWidgetContainer->setParent(vmeshTabber);
		vmeshTabber->addTab(ui.vmeshRenderWidgetContainer, tr("Rendering"));

		ui.vmeshSelFaceWidget->setParent(vmeshTabber);
		vmeshTabber->addTab(ui.vmeshSelFaceWidget, tr("Selected Face"));

		ui.vmeshRenderWidgetContainerLayout->setContentsMargins(4, 4, 4, 4);
		ui.vmeshSelFaceWidgetLayout->setContentsMargins(4, 4, 4, 4);

		ui.vmeshRenderWidgetContainer->show();
		ui.vmeshSelFaceWidget->show();

		ui.vmeshRenderGroupBox->hide();
		ui.vmeshSelFaceGroupBox->hide();

		vmeshTabber->show();


		// Shadow Mesh
		ui.smeshGeneralInfoWidget->setParent(smeshTabber);
		smeshTabber->addTab(ui.smeshGeneralInfoWidget, tr("General Information"));

		ui.smeshRenderWidgetContainer->setParent(smeshTabber);
		smeshTabber->addTab(ui.smeshRenderWidgetContainer, tr("Rendering"));

		ui.smeshSelFaceWidget->setParent(smeshTabber);
		smeshTabber->addTab(ui.smeshSelFaceWidget, tr("Selected Face"));

		ui.smeshGeneralInfoWidgetLayout->setContentsMargins(4, 4, 4, 4);
		ui.smeshRenderWidgetContainerLayout->setContentsMargins(4, 4, 4, 4);
		ui.smeshSelFaceWidgetLayout->setContentsMargins(4, 4, 4, 4);

		ui.smeshGeneralInfoWidget->show();
		ui.smeshRenderWidgetContainer->show();
		ui.smeshSelFaceWidget->show();

		ui.smeshGeneralInfoGroupBox->hide();
		ui.smeshRenderGroupBox->hide();
		ui.smeshSelFaceGroupBox->hide();

		// TODO: This is a hack. Without it, some graphical problems occur on the smeshTabber
		smeshTabber->setCurrentWidget(ui.smeshRenderWidgetContainer);
		smeshTabber->setCurrentWidget(ui.smeshSelFaceWidget);
		smeshTabber->setCurrentWidget(ui.smeshGeneralInfoWidget);

		smeshTabber->show();
	} else if (!compact &&  currentlyCompact) {
		// Spheres and Boxes
		sphereBoxTabber->removeTab(sphereBoxTabber->indexOf(ui.sphereBoxRenderWidgetContainer));
		sphereBoxTabber->removeTab(sphereBoxTabber->indexOf(ui.sphereBoxGeneralInfoWidget));

		ui.sphereBoxRenderWidgetContainer->setParent(ui.sphereBoxRenderGroupBox);
		ui.sphereBoxRenderGroupBoxLayout->addWidget(ui.sphereBoxRenderWidgetContainer);

		ui.sphereBoxGeneralInfoWidget->setParent(ui.sphereBoxGeneralInfoGroupBox);
		ui.sphereBoxGeneralInfoGroupBoxLayout->addWidget(ui.sphereBoxGeneralInfoWidget);

		ui.sphereBoxRenderWidgetContainerLayout->setContentsMargins(0, 0, 0, 0);
		ui.sphereBoxGeneralInfoWidgetLayout->setContentsMargins(0, 0, 0, 0);

		ui.sphereBoxRenderWidgetContainer->show();
		ui.sphereBoxGeneralInfoWidget->show();
		ui.sphereBoxRenderGroupBox->show();
		ui.sphereBoxGeneralInfoGroupBox->show();

		sphereBoxTabber->hide();


		// Vertex Mesh
		vmeshTabber->removeTab(vmeshTabber->indexOf(ui.vmeshRenderWidgetContainer));
		vmeshTabber->removeTab(vmeshTabber->indexOf(ui.vmeshSelFaceWidget));

		ui.vmeshRenderWidgetContainer->setParent(ui.vmeshRenderGroupBox);
		ui.vmeshRenderGroupBoxLayout->addWidget(ui.vmeshRenderWidgetContainer);

		ui.vmeshSelFaceWidget->setParent(ui.vmeshSelFaceGroupBox);
		ui.vmeshSelFaceGroupBoxLayout->addWidget(ui.vmeshSelFaceWidget);

		ui.vmeshRenderWidgetContainerLayout->setContentsMargins(0, 0, 0, 0);
		ui.vmeshSelFaceWidgetLayout->setContentsMargins(0, 0, 0, 0);

		ui.vmeshRenderWidgetContainer->show();
		ui.vmeshSelFaceWidget->show();
		ui.vmeshRenderGroupBox->show();
		ui.vmeshSelFaceGroupBox->show();

		vmeshTabber->hide();


		// Shadow Mesh
		smeshTabber->removeTab(smeshTabber->indexOf(ui.smeshGeneralInfoWidget));
		smeshTabber->removeTab(smeshTabber->indexOf(ui.smeshRenderWidgetContainer));
		smeshTabber->removeTab(smeshTabber->indexOf(ui.smeshSelFaceWidget));

		ui.smeshGeneralInfoWidget->setParent(ui.smeshGeneralInfoGroupBox);
		ui.smeshGeneralInfoGroupBoxLayout->addWidget(ui.smeshGeneralInfoWidget);

		ui.smeshRenderWidgetContainer->setParent(ui.smeshRenderGroupBox);
		ui.smeshRenderGroupBoxLayout->addWidget(ui.smeshRenderWidgetContainer);

		ui.smeshSelFaceWidget->setParent(ui.smeshSelFaceGroupBox);
		ui.smeshSelFaceGroupBoxLayout->addWidget(ui.smeshSelFaceWidget);

		ui.smeshGeneralInfoWidgetLayout->setContentsMargins(0, 0, 0, 0);
		ui.smeshRenderWidgetContainerLayout->setContentsMargins(0, 0, 0, 0);
		ui.smeshSelFaceWidgetLayout->setContentsMargins(0, 0, 0, 0);

		ui.smeshGeneralInfoWidget->show();
		ui.smeshRenderWidgetContainer->show();
		ui.smeshSelFaceWidget->show();
		ui.smeshGeneralInfoGroupBox->show();
		ui.smeshRenderGroupBox->show();
		ui.smeshSelFaceGroupBox->show();

		smeshTabber->hide();
	}

	currentlyCompact = compact;
}


void COLWidget::currentModelChanged(int index)
{
	COLModel* model = models[index];

	const COLBounds& bounds = model->getBounds();
	const Vector3& bsCenter = bounds.getCenter();
	float bsRadius = bounds.getRadius();
	const Vector3& bbMin = bounds.getMinimum();
	const Vector3& bbMax = bounds.getMaximum();

	/*ui.boundingSphereLabel->setText(QString("(%1, %2, %3 ; %4)").arg(bsCenter[0]).arg(bsCenter[1])
			.arg(bsCenter[2]).arg(bsRadius));*/
	ui.boundingSphereLabel->setText("Abrakadabra");
	/*ui.boundingBoxLabel->setText(QString("(%1, %2, %3)\n(%4, %5, %6)").arg(bbMin[0]).arg(bbMin[1])
			.arg(bbMin[2]).arg(bbMax[0]).arg(bbMax[1]).arg(bbMax[2]));*/
	ui.boundingBoxLabel->setText(QString("AAAAAA BBBBBBBBBB CCCCCCCC DDDDDDD EEEEEEEEE FFFFFFFFFF GGGGGGGGGG"));
	ui.sphereCountLabel->setText(QString("%1").arg(model->getSphereCount()));
	ui.boxCountLabel->setText(QString("%1").arg(model->getBoxCount()));
	ui.faceCountLabel->setText(QString("%1").arg(model->getFaceCount()));
	ui.vertexCountLabel->setText(QString("%1 aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa").arg(model->getVertexCount()));
	ui.faceGroupCountLabel->setText(QString("%1").arg(model->getFaceGroupCount()));
	ui.shadowMeshLabel->setText(model->getShadowMesh() == NULL ? tr("no") : tr("yes"));

	ui.meshList->clear();
	ui.faceGroupList->clear();

	ui.tabber->setTabEnabled(ui.tabber->indexOf(ui.sphereBoxTab),
			model->getSphereCount() > 0  ||  model->getBoxCount() > 0);

	for (uint32_t i = 0 ; i < model->getSphereCount() ; i++) {
		ui.meshList->addItem(QString("Sphere %1").arg(i+1));
	}

	for (uint32_t i = 0 ; i < model->getBoxCount() ; i++) {
		ui.meshList->addItem(QString("Box %1").arg(i+1));
	}

	const COLFaceGroup* faceGroups = model->getFaceGroups();

	if (faceGroups) {
		for (uint32_t i = 0 ; i < model->getFaceGroupCount() ; i++) {
			ui.faceGroupList->addItem(QString("Face Group %1").arg(i+1));
		}
	}

	const COLShadowMesh* shadowMesh = model->getShadowMesh();

	if (shadowMesh) {
		ui.tabber->setTabEnabled(ui.tabber->indexOf(ui.smeshTab), true);
		ui.smeshVertexCountLabel->setText(QString("%1").arg(shadowMesh->getVertexCount()));
		ui.smeshFaceCountLabel->setText(QString("%1").arg(shadowMesh->getFaceCount()));

		shadowMeshRenderer->render(shadowMesh->getVertices(), shadowMesh->getVertexCount(),
				shadowMesh->getFaces(), shadowMesh->getFaceCount());
	} else {
		ui.tabber->setTabEnabled(ui.tabber->indexOf(ui.smeshTab), false);
	}

	updateVertexMesh();
}


void COLWidget::faceGroupsToggled(bool status)
{
	ui.faceGroupList->setEnabled(status);
	updateVertexMesh();
}


void COLWidget::faceGroupSelectionChanged()
{
	updateVertexMesh();
}


void COLWidget::currentMeshChanged(int index)
{
	if (index == -1)
		return;

	COLModel* model = models[ui.modelList->currentRow()];

	if (index < (int) model->getSphereCount()) {
		const COLSphere& sphere = model->getSpheres()[index];
		const Vector3& center = sphere.getCenter();
		float radius = sphere.getRadius();
		const COLSurface& surface = sphere.getSurface();

		ui.sphereCenterLabel->setText(QString("(%1, %2, %3)").arg(center[0]).arg(center[1]).arg(center[2]));
		ui.sphereRadiusLabel->setText(QString("%1").arg(radius));
		ui.sphereMaterialLabel->setText(QString("%1").arg(surface.getMaterial()));
		ui.sphereFlagsLabel->setText(QString("%1").arg(surface.getFlags()));
		ui.sphereBrightnessLabel->setText(QString("%1").arg(surface.getBrightness()));
		ui.sphereLightLabel->setText(QString("%1").arg(surface.getLight()));
		ui.sphereBoxStackWidget->setCurrentWidget(ui.sphereWidget);

		sphereBoxRenderer->clear();
		sphereBoxRenderer->addSphere(sphere);
		sphereBoxRenderer->updateGL();
	} else {
		const COLBox& box = model->getBoxes()[index - model->getSphereCount()];
		const Vector3& min = box.getMinimum();
		const Vector3& max = box.getMaximum();
		const COLSurface& surface = box.getSurface();

		ui.boxMinLabel->setText(QString("(%1, %2, %3)").arg(min[0]).arg(min[1]).arg(min[2]));
		ui.boxMaxLabel->setText(QString("(%1, %2, %3)").arg(max[0]).arg(max[1]).arg(max[2]));
		ui.boxMaterialLabel->setText(QString("%1").arg(surface.getMaterial()));
		ui.boxFlagsLabel->setText(QString("%1").arg(surface.getFlags()));
		ui.boxBrightnessLabel->setText(QString("%1").arg(surface.getBrightness()));
		ui.boxLightLabel->setText(QString("%1").arg(surface.getLight()));
		ui.sphereBoxStackWidget->setCurrentWidget(ui.boxWidget);

		sphereBoxRenderer->clear();
		sphereBoxRenderer->addBox(box);
		sphereBoxRenderer->updateGL();
	}
}


void COLWidget::shadowMeshFaceSelectionChanged(int prevIdx, int idx)
{
	COLModel* model = models[ui.modelList->currentRow()];

	if (idx != -1) {
		const COLFace& face = model->getShadowMesh()->getFaces()[idx];
		const uint32_t* indices = face.getIndices();
		const COLSurface& surf = face.getSurface();

		ui.smeshFaceIndicesLabel->setText(QString("%1, %2, %3").arg(indices[0]).arg(indices[1])
				.arg(indices[2]));
		ui.smeshFaceMaterialLabel->setText(QString("%1").arg(surf.getMaterial()));
		ui.smeshFaceFlagsLabel->setText(QString("%1").arg(surf.getFlags()));
		ui.smeshFaceBrightnessLabel->setText(QString("%1").arg(surf.getBrightness()));
		ui.smeshFaceLightLabel->setText(QString("%1").arg(surf.getLight()));
	} else {
		ui.smeshFaceIndicesLabel->setText("-");
		ui.smeshFaceMaterialLabel->setText("-");
		ui.smeshFaceFlagsLabel->setText("-");
		ui.smeshFaceBrightnessLabel->setText("-");
		ui.smeshFaceLightLabel->setText("-");
	}
}


void COLWidget::vertexMeshFaceSelectionChanged(int prevIdx, int idx)
{
	COLModel* model = models[ui.modelList->currentRow()];

	if (idx != -1) {
		idx = vmeshFaceIndexMap[idx];
		const COLFace& face = model->getFaces()[idx];
		const uint32_t* indices = face.getIndices();
		const COLSurface& surf = face.getSurface();

		ui.vmeshFaceIndicesLabel->setText(QString("%1, %2, %3").arg(indices[0]).arg(indices[1])
				.arg(indices[2]));
		ui.vmeshFaceMaterialLabel->setText(QString("%1").arg(surf.getMaterial()));
		ui.vmeshFaceFlagsLabel->setText(QString("%1").arg(surf.getFlags()));
		ui.vmeshFaceBrightnessLabel->setText(QString("%1").arg(surf.getBrightness()));
		ui.vmeshFaceLightLabel->setText(QString("%1").arg(surf.getLight()));
	} else {
		ui.vmeshFaceIndicesLabel->setText("-");
		ui.vmeshFaceMaterialLabel->setText("-");
		ui.vmeshFaceFlagsLabel->setText("-");
		ui.vmeshFaceBrightnessLabel->setText("-");
		ui.vmeshFaceLightLabel->setText("-");
	}
}


void COLWidget::updateVertexMesh()
{
	return;
	meshRenderer->setSelectedFace(-1);
	vmeshFaceIndexMap.clear();

	int idx = ui.modelList->currentRow();

	if (idx == -1)
		return;

	COLModel* model = models[ui.modelList->currentRow()];
	uint32_t faceCount = model->getFaceCount();

	COLFace* selFaces = new COLFace[faceCount];
	uint32_t selFaceCount = 0;

	if (ui.faceGroupBox->isChecked()) {
		QLinkedList<COLFaceGroup*> selFaceGroups;
		QList<QListWidgetItem*> selItems = ui.faceGroupList->selectedItems();
		QList<QListWidgetItem*>::iterator it;

		for (it = selItems.begin() ; it != selItems.end() ; it++) {
			QListWidgetItem* item = *it;
			int idx = ui.faceGroupList->row(item);
			selFaceGroups << model->getFaceGroups()+idx;
		}

		for (uint32_t i = 0 ; i < faceCount ; i++) {
			QLinkedList<COLFaceGroup*>::iterator fgit;
			bool selected = false;

			for (fgit = selFaceGroups.begin() ; fgit != selFaceGroups.end() ; fgit++) {
				COLFaceGroup* faceGroup = *fgit;
				uint16_t start = faceGroup->getStartFace();
				uint16_t end = faceGroup->getEndFace();

				if (i >= start  &&  i < end) {
					selected = true;
					break;
				}
			}

			if (selected) {
				selFaces[selFaceCount++] = model->getFaces()[i];
				vmeshFaceIndexMap << i;
			}
		}
	} else {
		memcpy(selFaces, model->getFaces(), faceCount * sizeof(COLFace));
		selFaceCount = faceCount;

		for (uint32_t i = 0 ; i < faceCount ; i++) {
			vmeshFaceIndexMap << i;
		}
	}

	meshRenderer->render(model->getVertices(), model->getVertexCount(), selFaces, selFaceCount);

	delete[] selFaces;
}

