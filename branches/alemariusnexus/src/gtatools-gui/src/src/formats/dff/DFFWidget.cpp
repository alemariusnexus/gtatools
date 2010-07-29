/*
 * DFFWidget.cpp
 *
 *  Created on: 20.07.2010
 *      Author: alemariusnexus
 */

#include "DFFWidget.h"
#include <qlistwidget.h>
#include "../../System.h"



DFFWidget::DFFWidget(const File& file, QWidget* parent)
		: QWidget(parent)
{
	ui.setupUi(this);

	geometryRenderWidget = new DFFRenderWidget(ui.geometryRenderContainerWidget);
	ui.geometryRenderContainerWidget->layout()->addWidget(geometryRenderWidget);

	geometryPartRenderWidget = new DFFRenderWidget(ui.geometryPartRenderContainerWidget);
	ui.geometryPartRenderContainerWidget->layout()->addWidget(geometryPartRenderWidget);

	DFFLoader dff;
	mesh = dff.loadMesh(file);


	DFFFrame** frames = mesh->getFrames();

	int numUnnamed = 0;
	for (int i = 0 ; i < mesh->getFrameCount() ; i++) {
		DFFFrame* frame = frames[i];
		QString fname;

		if (frame->getName() == NULL) {
			fname = tr("Unnamed %1").arg(++numUnnamed);
		} else {
			fname = frame->getName();
		}

		QListWidgetItem* item = new QListWidgetItem(fname);
		ui.frameList->addItem(item);
	}


	DFFGeometry** geoms = mesh->getGeometries();

	for (int i = 0 ; i < mesh->getGeometryCount() ; i++) {
		DFFGeometry* geom = geoms[i];
		QListWidgetItem* item = new QListWidgetItem(tr("Geometry %1").arg(i+1));
		ui.geometryList->addItem(item);
	}

	guiModule = new DFFGUIModule(this);

	System::getInstance()->installGUIModule(guiModule);


	connect(ui.frameList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(frameSelected(QListWidgetItem*)));
	connect(ui.geometryList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(geometrySelected(QListWidgetItem*)));
	connect(ui.materialList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(materialSelected(QListWidgetItem*)));
	connect(ui.textureList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(textureSelected(QListWidgetItem*)));
	connect(ui.geometryPartList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(geometryPartSelected(QListWidgetItem*)));
}


DFFWidget::~DFFWidget()
{
	System::getInstance()->uninstallGUIModule(guiModule);
	delete guiModule;

	clearMaterialList();
	clearGeometryPartList();

	int count = ui.frameList->count();

	for (int i = 0 ; i < count ; i++) {
		delete ui.frameList->item(0);
	}

	count = ui.geometryList->count();

	for (int i = 0 ; i < count ; i++) {
		delete ui.geometryList->item(0);
	}

	delete mesh;
}


void DFFWidget::clearMaterialList()
{
	clearTextureList();

	int count = ui.materialList->count();

	for (int i = 0 ; i < count ; i++) {
		delete ui.materialList->item(0);
	}
}


void DFFWidget::clearGeometryPartList()
{
	int count = ui.geometryPartList->count();

	for (int i = 0 ; i < count ; i++) {
		delete ui.geometryPartList->item(0);
	}
}


void DFFWidget::clearTextureList()
{
	int count = ui.textureList->count();

	for (int i = 0 ; i < count ; i++) {
		delete ui.textureList->item(0);
	}
}


void DFFWidget::frameSelected(QListWidgetItem* item)
{
	DFFFrame* frame = mesh->getFrames()[ui.frameList->row(item)];

	const float* trans = frame->getTranslation();
	const float* rot = frame->getRotation();

	ui.frameNameLabel->setText(item->text());
	ui.frameTranslationLabel->setText(tr("(%1, %2, %3)").arg(trans[0]).arg(trans[1]).arg(trans[2]));
	ui.frameRotationLabel->setText(tr("(%1, %2, %3) (%4, %5, %6) (%7, %8, %9)")
			.arg(rot[0]).arg(rot[1]).arg(rot[2]).arg(rot[3]).arg(rot[4]).arg(rot[5]).arg(rot[6]).arg(rot[7]).arg(rot[8]));

	if (frame->getParent() == NULL) {
		ui.frameParentLabel->setText(tr("None"));
	} else {
		int pidx = mesh->indexOf(frame->getParent());
		ui.frameParentLabel->setText(ui.frameList->item(pidx)->text());
	}

	ui.frameFlagsLabel->setText(QString("%1").arg(frame->getFlags()));
}


void DFFWidget::geometrySelected(QListWidgetItem* item)
{
	DFFGeometry* geom = mesh->getGeometries()[ui.geometryList->row(item)];

	const DFFBoundingSphere* bounds = geom->getBounds();

	ui.geometryNameLabel->setText(item->text());
	ui.geometryFaceFormatLabel->setText(geom->isTriangleStripFormat() ? tr("Triangle Strips") : tr("Triangle List"));
	ui.geometryFlagsLabel->setText(QString("%1").arg(geom->getFlags()));
	ui.geometryUVSetCountLabel->setText(QString("%1").arg(geom->getUVSetCount()));
	ui.geometryVertexCountLabel->setText(QString("%1").arg(geom->getVertexCount()));
	ui.geometryFrameCountLabel->setText(QString("%1").arg(geom->getFrameCount()));
	ui.geometryAmbientLightLabel->setText(QString("%1").arg(geom->getAmbientLight()));
	ui.geometryDiffuseLightLabel->setText(QString("%1").arg(geom->getDiffuseLight()));
	ui.geometrySpecularLightLabel->setText(QString("%1").arg(geom->getSpecularLight()));
	ui.geometryVertexColorsLabel->setText(geom->getVertexColors() == NULL ? tr("no") : tr("yes"));
	ui.geometryNormalsLabel->setText(geom->getNormals() == NULL ? tr("no") : tr("yes"));
	ui.geometryBoundsLabel->setText(tr("(%1, %2, %3 : %4)").arg(bounds->x).arg(bounds->y).arg(bounds->z).arg(bounds->radius));
	ui.geometryMaterialCountLabel->setText(QString("%1").arg(geom->getMaterialCount()));
	ui.geometryPartCountLabel->setText(QString("%1").arg(geom->getPartCount()));
	ui.geometryFrameLabel->setText(ui.frameList->item(mesh->indexOf(geom->getAssociatedFrame()))->text());

	clearMaterialList();
	clearGeometryPartList();

	DFFMaterial** mats = geom->getMaterials();

	for (int i = 0 ; i < geom->getMaterialCount() ; i++) {
		DFFMaterial* mat = mats[i];
		QListWidgetItem* item = new QListWidgetItem(tr("Material %1").arg(i+1));
		ui.materialList->addItem(item);
	}

	DFFGeometryPart** parts = geom->getParts();

	for (int i = 0 ; i < geom->getPartCount() ; i++) {
		DFFGeometryPart* part = parts[i];
		QListWidgetItem* item = new QListWidgetItem(tr("Part %1").arg(i+1));
		ui.geometryPartList->addItem(item);
	}

	geometryRenderWidget->renderGeometry(geom);
	geometryRenderWidget->updateGL();
}


void DFFWidget::materialSelected(QListWidgetItem* item)
{
	DFFGeometry* geom = mesh->getGeometries()[ui.geometryList->currentRow()];
	DFFMaterial* mat = geom->getMaterials()[ui.materialList->row(item)];

	uint8_t r, g, b, a;
	mat->getColor(r, g, b, a);

	ui.materialNameLabel->setText(item->text());
	ui.materialColorLabel->setText(tr("(%1, %2, %3, %4)").arg(r).arg(g).arg(b).arg(a));
	ui.materialTextureCountLabel->setText(QString("%1").arg(mat->getTextureCount()));

	clearTextureList();

	DFFTexture** texes = mat->getTextures();

	for (int i = 0 ; i < mat->getTextureCount() ; i++) {
		DFFTexture* tex = texes[i];
		QListWidgetItem* item = new QListWidgetItem(tex->getDiffuseName());
		ui.textureList->addItem(item);
	}
}


void DFFWidget::textureSelected(QListWidgetItem* item)
{
	DFFGeometry* geom = mesh->getGeometries()[ui.geometryList->currentRow()];
	DFFMaterial* mat = geom->getMaterials()[ui.materialList->currentRow()];
	DFFTexture* tex = mat->getTextures()[ui.textureList->row(item)];

	ui.textureNameLabel->setText(item->text());
	ui.textureDiffuseNameLabel->setText(tex->getDiffuseName());
	ui.textureAlphaNameLabel->setText(tex->getAlphaName());
	ui.textureFilterFlagsLabel->setText(QString("%1").arg(tex->getFilterModeFlags()));
}


void DFFWidget::geometryPartSelected(QListWidgetItem* item)
{
	DFFGeometry* geom = mesh->getGeometries()[ui.geometryList->currentRow()];
	DFFGeometryPart* part = geom->getParts()[ui.geometryPartList->row(item)];

	ui.geometryPartNameLabel->setText(item->text());
	ui.geometryPartIndexCountLabel->setText(QString("%1").arg(part->getIndexCount()));
	ui.geometryPartMaterialLabel->setText(ui.materialList->item(geom->indexOf(part->getMaterial()))->text());

	geometryPartRenderWidget->renderGeometryPart(geom, part);
	geometryPartRenderWidget->updateGL();
}


void DFFWidget::texturedPropertyChanged(bool textured)
{
	geometryRenderWidget->setShowTextures(textured);
	geometryPartRenderWidget->setShowTextures(textured);
}


void DFFWidget::wireframePropertyChanged(bool wireframe)
{
	geometryRenderWidget->setShowWireframe(wireframe);
	geometryPartRenderWidget->setShowWireframe(wireframe);
}

