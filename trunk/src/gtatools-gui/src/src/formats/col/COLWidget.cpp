#include "COLWidget.h"
#include "COLGUIModule.h"
#include "../../System.h"



COLWidget::COLWidget(const File& file, QWidget *parent)
		: QWidget(parent)
{
	ui.setupUi(this);

	sphereBoxRenderer = new COLSphereBoxRenderWidget(ui.sphereBoxRenderWidgetContainer);
	ui.sphereBoxRenderWidgetContainer->layout()->addWidget(sphereBoxRenderer);

	shadowMeshRenderer = new COLMeshRenderWidget(ui.shadowMeshRenderWidgetContainer);
	ui.shadowMeshRenderWidgetContainer->layout()->addWidget(shadowMeshRenderer);

	meshRenderer = new COLMeshRenderWidget(ui.vertexMeshRenderWidgetContainer);
	ui.vertexMeshRenderWidgetContainer->layout()->addWidget(meshRenderer);

	COLLoader col;
	istream* stream = file.openInputStream(istream::binary);

	COLModel* model;

	while ((model = col.loadModel(stream))) {
		models << model;
		ui.modelList->addItem(model->getName());
	}

	System::getInstance()->installGUIModule(COLGUIModule::getInstance());

	connect(ui.modelList, SIGNAL(currentRowChanged(int)), this, SLOT(currentModelChanged(int)));
	connect(ui.faceGroupBox, SIGNAL(toggled(bool)), this, SLOT(faceGroupsToggled(bool)));
	connect(ui.faceGroupList, SIGNAL(itemSelectionChanged()), this, SLOT(faceGroupSelectionChanged()));
	connect(ui.meshList, SIGNAL(currentRowChanged(int)), this, SLOT(currentMeshChanged(int)));
	connect(COLGUIModule::getInstance(), SIGNAL(wireframePropertyChanged(bool)), this,
			SLOT(wireframePropertyChanged(bool)));
	connect(shadowMeshRenderer, SIGNAL(faceSelectionChanged(int, int)), this,
			SLOT(shadowMeshFaceSelectionChanged(int, int)));
	connect(meshRenderer, SIGNAL(faceSelectionChanged(int, int)), this,
			SLOT(vertexMeshFaceSelectionChanged(int, int)));

	ui.faceGroupBox->setChecked(false);
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

	ui.meshList->clear();
	ui.faceGroupList->clear();

	ui.tabber->setTabEnabled(ui.tabber->indexOf(ui.sphereBoxTab),
			model->getSphereCount() > 0  ||  model->getBoxCount() > 0);

	const COLSphere* spheres = model->getSpheres();

	for (uint32_t i = 0 ; i < model->getSphereCount() ; i++) {
		const COLSphere& sphere = spheres[i];
		ui.meshList->addItem(QString("Sphere %1").arg(i+1));
	}

	const COLBox* boxes = model->getBoxes();

	for (uint32_t i = 0 ; i < model->getBoxCount() ; i++) {
		const COLBox& box = boxes[i];
		ui.meshList->addItem(QString("Box %1").arg(i+1));
	}

	const COLFaceGroup* faceGroups = model->getFaceGroups();

	if (faceGroups) {
		for (uint32_t i = 0 ; i < model->getFaceGroupCount() ; i++) {
			const COLFaceGroup& faceGroup = faceGroups[i];
			ui.faceGroupList->addItem(QString("Face Group %1").arg(i+1));
		}
	}

	const COLShadowMesh* shadowMesh = model->getShadowMesh();

	if (shadowMesh) {
		ui.tabber->setTabEnabled(ui.tabber->indexOf(ui.shadowMeshTab), true);
		ui.smeshVertexCountLabel->setText(QString("%1").arg(shadowMesh->getVertexCount()));
		ui.smeshFaceCountLabel->setText(QString("%1").arg(shadowMesh->getFaceCount()));

		shadowMeshRenderer->render(shadowMesh->getVertices(), shadowMesh->getVertexCount(),
				shadowMesh->getFaces(), shadowMesh->getFaceCount());
	} else {
		ui.tabber->setTabEnabled(ui.tabber->indexOf(ui.shadowMeshTab), false);
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

	if (index < model->getSphereCount()) {
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


void COLWidget::wireframePropertyChanged(bool wireframe)
{
	sphereBoxRenderer->setWireframe(wireframe);
}


void COLWidget::shadowMeshFaceSelectionChanged(int prevIdx, int idx)
{
	COLModel* model = models[ui.modelList->currentRow()];

	if (idx != -1) {
		const COLFace& face = model->getShadowMesh()->getFaces()[idx];
		const uint32_t* indices = face.getIndices();
		const COLSurface& surf = face.getSurface();

		ui.smeshFaceIndicesLabel->setText(QString("%1, %2, %3").arg(indices[0]).arg(indices[1]).arg(indices[2]));
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

		ui.vmeshFaceIndicesLabel->setText(QString("%1, %2, %3").arg(indices[0]).arg(indices[1]).arg(indices[2]));
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

