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


#include "COLWidget.h"
#include "COLGUIModule.h"
#include "../../System.h"
#include <QtCore/QSettings>



COLWidget::COLWidget(const File& file, QWidget *parent)
		: QWidget(parent), currentlyCompact(false), entityModel(NULL)
{
	ui.setupUi(this);

	ui.collisionMeshMainSplitter->setSizes(QList<int>() << width() / 3 << (width() / 3) * 2);

	sphereBoxTabber = new QTabWidget;
	sphereBoxTabber->setParent(ui.sphereBoxInfoWidget);
	ui.sphereBoxInfoWidgetLayout->addWidget(sphereBoxTabber);

	vmeshTabber = new QTabWidget;
	vmeshTabber->setParent(ui.vmeshInfoWidget);
	ui.vmeshInfoWidgetLayout->addWidget(vmeshTabber);

	smeshTabber = new QTabWidget;
	smeshTabber->setParent(ui.smeshTab);
	ui.smeshTabLayout->addWidget(smeshTabber);

	renderContainer = new GLContainerWidget(ui.renderWidgetContainer);
	ui.renderWidgetContainer->layout()->addWidget(renderContainer);
	renderWidget = new COLRenderWidget(renderContainer);
	renderContainer->setGLWidget(renderWidget);

	COLLoader col;
	istream* stream = file.openInputStream(istream::binary);

	COLModel* model;

	while ((model = col.loadModel(stream))) {
		models << model;
		ui.modelList->addItem(model->getName().get());
	}

	System* sys = System::getInstance();

	sys->installGUIModule(COLGUIModule::getInstance());

	connect(ui.collisionMeshMainSplitter, SIGNAL(splitterMoved(int, int)),
			this, SLOT(collisionMeshMainSplitterValueChanged(int, int)));
	connect(ui.modelList, SIGNAL(currentRowChanged(int)), this, SLOT(currentModelChanged(int)));
	//connect(ui.meshList, SIGNAL(currentRowChanged(int)), this, SLOT(currentMeshChanged(int)));
	/*connect(meshRenderer, SIGNAL(faceSelectionChanged(int, int)), this,
			SLOT(vertexMeshFaceSelectionChanged(int, int)));*/
	connect(sys, SIGNAL(configurationChanged()), this, SLOT(updateLayoutType()));

	updateLayoutType();

	loadConfigUiSettings();
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


void COLWidget::loadConfigUiSettings()
{
	QSettings settings;

	ui.collisionMeshMainSplitter->restoreState(settings
			.value("gui_geometry_COLWidget/collisionMeshMainSplitter_state").toByteArray());
}


void COLWidget::updateLayoutType()
{
	QSettings settings;

	bool compact = settings.value("gui/compact_mode", false).toBool();

	/*if (compact  &&  !currentlyCompact) {
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

	currentlyCompact = compact;*/
}


void COLWidget::currentModelChanged(int index)
{
	COLModel* model = models[index];

	const COLBounds& bounds = model->getBounds();
	const Vector3& bsCenter = bounds.getCenter();
	float bsRadius = bounds.getRadius();
	const Vector3& bbMin = bounds.getMinimum();
	const Vector3& bbMax = bounds.getMaximum();

	ui.boundingSphereLabel->setText(QString("(%1, %2, %3 ; %4)").arg(bsCenter[0]).arg(bsCenter[1])
			.arg(bsCenter[2]).arg(bsRadius));
	ui.boundingBoxLabel->setText(QString("(%1, %2, %3)\n(%4, %5, %6)").arg(bbMin[0]).arg(bbMin[1])
			.arg(bbMin[2]).arg(bbMax[0]).arg(bbMax[1]).arg(bbMax[2]));
	ui.sphereCountLabel->setText(QString("%1").arg(model->getSphereCount()));
	ui.boxCountLabel->setText(QString("%1").arg(model->getBoxCount()));
	ui.faceCountLabel->setText(QString("%1").arg(model->getFaceCount()));
	ui.vertexCountLabel->setText(QString("%1").arg(model->getVertexCount()));
	ui.faceGroupCountLabel->setText(QString("%1").arg(model->getFaceGroupCount()));
	ui.shadowMeshLabel->setText(model->getShadowMesh() == NULL ? tr("no") : tr("yes"));

	renderWidget->displayModel(model);

	renderWidget->update();

	COLEntityItemModel* newModel = new COLEntityItemModel(model);
	ui.entityTree->setModel(NULL);
	ui.entityTree->setModel(newModel);

	delete entityModel;

	entityModel = newModel;

	ui.entityTree->update();

	/*ui.meshList->clear();
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

	updateVertexMesh();*/
}


/*void COLWidget::updateVertexMesh()
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
}*/


void COLWidget::collisionMeshMainSplitterValueChanged(int pos, int idx)
{
	QSettings settings;

	settings.setValue("gui_geometry_COLWidget/collisionMeshMainSplitter_state",
			ui.collisionMeshMainSplitter->saveState());
}

