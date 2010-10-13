/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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
 */

#include "DFFWidget.h"
#include <qlistwidget.h>
#include "../../System.h"
#include <qfiledialog.h>
#include "DFFXMLDumpDialog.h"
#include <qfile.h>
#include <qtextstream.h>
#include "DFFFormatHandler.h"


int mainTabberIndex = 0;
int geometryTabberIndex = 0;
int geometryPartTabberIndex = 0;


void frameRecurse(DFFFrame* parent, int numInd)
{
	DFFFrame::ChildIterator it;

	for (it = parent->getChildBegin() ; it != parent->getChildEnd() ; it++) {
		DFFFrame* frame = *it;

		for (int i = 0 ; i < numInd ; i++) {
			printf("  ");
		}

		if (frame->getName()) {
			printf("%s\n", frame->getName());
		} else {
			printf("(Unnamed)\n");
		}

		frameRecurse(frame, numInd+1);
	}
}



DFFWidget::DFFWidget(const File& file, QWidget* parent, QGLWidget* shareWidget)
		: QWidget(parent)
{
	ui.setupUi(this);

	ui.mainTabber->setCurrentIndex(mainTabberIndex);
	ui.geometryTabber->setCurrentIndex(geometryTabberIndex);
	ui.geometryPartTabber->setCurrentIndex(geometryPartTabberIndex);

	geometryRenderWidget = new DFFRenderWidget(ui.geometryRenderContainerWidget, shareWidget);
	ui.geometryRenderContainerWidget->layout()->addWidget(geometryRenderWidget);

	geometryPartRenderWidget = new DFFRenderWidget(ui.geometryPartRenderContainerWidget, shareWidget);
	ui.geometryPartRenderContainerWidget->layout()->addWidget(geometryPartRenderWidget);

	DFFLoader dff;
	mesh = dff.loadMesh(file);

	frameModel = new DFFFrameItemModel(mesh);
	ui.frameTree->setModel(frameModel);


	DFFMesh::GeometryIterator git;

	int i = 0;
	for (git = mesh->getGeometryBegin() ; git != mesh->getGeometryEnd() ; git++, i++) {
		DFFGeometry* geom = *git;
		QListWidgetItem* item = new QListWidgetItem(tr("Geometry %1").arg(i+1));
		ui.geometryList->addItem(item);
	}

	guiModule = new DFFGUIModule(this);

	System::getInstance()->installGUIModule(guiModule);


	connect(ui.frameTree->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
			this, SLOT(frameSelected(const QModelIndex&, const QModelIndex&)));
	connect(ui.geometryList, SIGNAL(currentRowChanged(int)), this, SLOT(geometrySelected(int)));
	connect(ui.materialList, SIGNAL(currentRowChanged(int)), this, SLOT(materialSelected(int)));
	connect(ui.textureList, SIGNAL(currentRowChanged(int)), this, SLOT(textureSelected(int)));
	connect(ui.geometryPartList, SIGNAL(currentRowChanged(int)), this, SLOT(geometryPartSelected(int)));

	geometryRenderWidget->updateGL();
	geometryPartRenderWidget->updateGL();

	ui.geometryList->setCurrentRow(0);
}


DFFWidget::~DFFWidget()
{
	mainTabberIndex = ui.mainTabber->currentIndex();
	geometryTabberIndex = ui.geometryTabber->currentIndex();
	geometryPartTabberIndex = ui.geometryPartTabber->currentIndex();

	System::getInstance()->uninstallGUIModule(guiModule);
	delete guiModule;

	clearGeometryPartList();
	clearMaterialList();

	int count = ui.geometryList->count();

	for (int i = 0 ; i < count ; i++) {
		delete ui.geometryList->takeItem(0);
	}

	delete mesh;

	delete frameModel;

	delete geometryRenderWidget;
	delete geometryPartRenderWidget;
}


void DFFWidget::clearMaterialList()
{
	// Otherwise the slots might be called in an invalid state
	disconnect(ui.materialList, SIGNAL(currentRowChanged(int)), this, SLOT(materialSelected(int)));

	clearTextureList();

	int count = ui.materialList->count();

	for (int i = 0 ; i < count ; i++) {
		delete ui.materialList->takeItem(0);
	}

	connect(ui.materialList, SIGNAL(currentRowChanged(int)), this, SLOT(materialSelected(int)));
}


void DFFWidget::clearGeometryPartList()
{
	// Otherwise the slots might be called in an invalid state
	disconnect(ui.geometryPartList, SIGNAL(currentRowChanged(int)), this, SLOT(geometryPartSelected(int)));

	int count = ui.geometryPartList->count();

	for (int i = 0 ; i < count ; i++) {
		delete ui.geometryPartList->takeItem(0);
	}

	connect(ui.geometryPartList, SIGNAL(currentRowChanged(int)), this, SLOT(geometryPartSelected(int)));
}


void DFFWidget::clearTextureList()
{
	// Otherwise the slots might be called in an invalid state
	disconnect(ui.textureList, SIGNAL(currentRowChanged(int)), this, SLOT(textureSelected(int)));

	int count = ui.textureList->count();

	for (int i = 0 ; i < count ; i++) {
		delete ui.textureList->takeItem(0);
	}

	connect(ui.textureList, SIGNAL(currentRowChanged(int)), this, SLOT(textureSelected(int)));
}


void DFFWidget::frameSelected(const QModelIndex& index, const QModelIndex& previous)
{
	DFFFrame* frame = (DFFFrame*) index.internalPointer();

	Vector3 trans = frame->getTranslation();
	Matrix3 rot = frame->getRotation();
	const float* arot = rot.toArray();

	ui.frameNameLabel->setText(index.data(Qt::DisplayRole).toString());
	ui.frameTranslationLabel->setText(tr("(%1, %2, %3)").arg(trans[0]).arg(trans[1]).arg(trans[2]));
	ui.frameRotationLabel->setText(tr("(%1, %2, %3) (%4, %5, %6) (%7, %8, %9)")
			.arg(arot[0]).arg(arot[1]).arg(arot[2]).arg(arot[3]).arg(arot[4]).arg(arot[5]).arg(arot[6])
			.arg(arot[7]).arg(arot[8]));

	if (frame->getParent() == NULL) {
		ui.frameParentLabel->setText(tr("None"));
	} else {
		ui.frameParentLabel->setText(index.parent().data(Qt::DisplayRole).toString());
	}

	ui.frameFlagsLabel->setText(QString("%1").arg(frame->getFlags()));
}


void DFFWidget::geometrySelected(int row)
{
	if (row == -1)
		return;

	DFFGeometry* geom = mesh->getGeometry(row);

	const DFFBoundingSphere* bounds = geom->getBounds();

	ui.geometryNameLabel->setText(ui.geometryList->item(row)->text());
	ui.geometryFaceFormatLabel->setText(geom->isTriangleStripFormat()
			? tr("Triangle Strips") : tr("Triangle List"));
	ui.geometryFlagsLabel->setText(QString("%1").arg(geom->getFlags()));
	ui.geometryUVSetCountLabel->setText(QString("%1").arg(geom->getUVSetCount()));
	ui.geometryVertexCountLabel->setText(QString("%1").arg(geom->getVertexCount()));
	ui.geometryFrameCountLabel->setText(QString("%1").arg(geom->getFrameCount()));
	ui.geometryAmbientLightLabel->setText(QString("%1").arg(geom->getAmbientLight()));
	ui.geometryDiffuseLightLabel->setText(QString("%1").arg(geom->getDiffuseLight()));
	ui.geometrySpecularLightLabel->setText(QString("%1").arg(geom->getSpecularLight()));
	ui.geometryVertexColorsLabel->setText(geom->getVertexColors() == NULL ? tr("no") : tr("yes"));
	ui.geometryNormalsLabel->setText(geom->getNormals() == NULL ? tr("no") : tr("yes"));
	ui.geometryBoundsLabel->setText(tr("(%1, %2, %3 : %4)")
			.arg(bounds->x).arg(bounds->y).arg(bounds->z).arg(bounds->radius));
	ui.geometryMaterialCountLabel->setText(QString("%1").arg(geom->getMaterialCount()));
	ui.geometryPartCountLabel->setText(QString("%1").arg(geom->getPartCount()));

	DFFFrame* frame = geom->getAssociatedFrame();

	QStringList framePath;

	if (frame) {
		do {
			if (frame->getName()) {
				framePath.insert(0, frame->getName());
			} else {
				if (!frame->isRoot()) {
					framePath.insert(0, QString("%1")
							.arg(frame->getParent()->indexOf(frame)));
				}/* else {
					framePath.insert(0, QString("%1").arg(mesh->indexOf(frame)));
				}*/
			}
		} while ((frame = frame->getParent())  !=  NULL);
	} else {
		framePath << tr("None");
	}

	ui.geometryFrameLabel->setText(framePath.join("/"));

	clearGeometryPartList();
	clearMaterialList();

	DFFGeometry::MaterialIterator mit;

	int i = 0;
	for (mit = geom->getMaterialBegin() ; mit != geom->getMaterialEnd() ; mit++, i++) {
		DFFMaterial* mat = *mit;
		QListWidgetItem* item = new QListWidgetItem(tr("Material %1").arg(i+1));
		ui.materialList->addItem(item);
	}

	DFFGeometry::PartIterator pit;

	i = 0;
	for (pit = geom->getPartBegin() ; pit != geom->getPartEnd() ; pit++, i++) {
		DFFGeometryPart* part = *pit;
		QListWidgetItem* item = new QListWidgetItem(tr("Part %1").arg(i+1));
		ui.geometryPartList->addItem(item);
	}

	ui.materialList->setCurrentRow(0);
	ui.geometryPartList->setCurrentRow(0);

	geometryRenderWidget->renderGeometry(geom);
	geometryRenderWidget->updateGL();
}


void DFFWidget::materialSelected(int row)
{
	if (row == -1)
		return;

	DFFGeometry* geom = mesh->getGeometry(ui.geometryList->currentRow());
	DFFMaterial* mat = geom->getMaterial(row);

	uint8_t r, g, b, a;
	mat->getColor(r, g, b, a);

	ui.materialNameLabel->setText(ui.materialList->item(row)->text());
	ui.materialColorLabel->setText(tr("(%1, %2, %3, %4)").arg(r).arg(g).arg(b).arg(a));
	ui.materialTextureCountLabel->setText(QString("%1").arg(mat->getTextureCount()));

	clearTextureList();

	DFFMaterial::TextureIterator it;

	for (it = mat->getTextureBegin() ; it != mat->getTextureEnd() ; it++) {
		DFFTexture* tex = *it;
		QListWidgetItem* item = new QListWidgetItem(tex->getDiffuseName());
		ui.textureList->addItem(item);
	}

	ui.textureList->setCurrentRow(0);
}


void DFFWidget::textureSelected(int row)
{
	if (row == -1)
		return;

	DFFGeometry* geom = mesh->getGeometry(ui.geometryList->currentRow());
	DFFMaterial* mat = geom->getMaterial(ui.materialList->currentRow());
	DFFTexture* tex = mat->getTexture(row);

	ui.textureNameLabel->setText(ui.textureList->item(row)->text());
	ui.textureDiffuseNameLabel->setText(tex->getDiffuseName());
	ui.textureAlphaNameLabel->setText(tex->getAlphaName());
	ui.textureFilterFlagsLabel->setText(QString("%1").arg(tex->getFilterModeFlags()));
}


void DFFWidget::geometryPartSelected(int row)
{
	if (row == -1)
		return;

	DFFGeometry* geom = mesh->getGeometry(ui.geometryList->currentRow());
	DFFGeometryPart* part = geom->getPart(row);
	DFFMaterial* mat = part->getMaterial();

	ui.geometryPartNameLabel->setText(ui.geometryPartList->item(row)->text());
	ui.geometryPartIndexCountLabel->setText(QString("%1").arg(part->getIndexCount()));

	if (mat) {
		QListWidgetItem* item = ui.materialList->item(geom->indexOf(mat));
		ui.geometryPartMaterialLabel->setText(ui.materialList->item(geom->indexOf(mat))->text());
	} else {
		ui.geometryPartMaterialLabel->setText(tr("None"));
	}

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


void DFFWidget::xmlDumpRequested(bool)
{
	DFFFormatHandler::getInstance()->xmlDumpDialog(*mesh, this);
}

