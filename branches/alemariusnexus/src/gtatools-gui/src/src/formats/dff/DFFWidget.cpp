/*
 * DFFWidget.cpp
 *
 *  Created on: 20.07.2010
 *      Author: alemariusnexus
 */

#include "DFFWidget.h"
#include <qlistwidget.h>
#include "../../System.h"
#include <qfiledialog.h>
#include "DFFXMLDumpDialog.h"
#include <qfile.h>
#include <qtextstream.h>



DFFWidget::DFFWidget(const File& file, QWidget* parent)
		: QWidget(parent)
{
	ui.setupUi(this);

	geometryRenderWidget = new DFFRenderWidget(ui.geometryRenderContainerWidget);
	ui.geometryRenderContainerWidget->layout()->addWidget(geometryRenderWidget);

	geometryPartRenderWidget = new DFFRenderWidget(ui.geometryPartRenderContainerWidget);
	ui.geometryPartRenderContainerWidget->layout()->addWidget(geometryPartRenderWidget);

	DFFLoader dff;
	dff.setVerbose(true);
	mesh = dff.loadMesh(file);


	/*DFFMesh::FrameIterator fit;

	int numUnnamed = 0;
	for (fit = mesh->getFrameBegin() ; fit != mesh->getFrameEnd() ; fit++) {
		DFFFrame* frame = *fit;
		QString fname;

		if (frame->getName() == NULL) {
			fname = tr("Unnamed %1").arg(++numUnnamed);
		} else {
			fname = frame->getName();
		}

		QListWidgetItem* item = new QListWidgetItem(fname);
		ui.frameList->addItem(item);
	}*/

	frameModel = new DFFFrameItemModel(mesh);
	ui.frameTable->setModel(frameModel);


	DFFMesh::GeometryIterator git;

	int i = 0;
	for (git = mesh->getGeometryBegin() ; git != mesh->getGeometryEnd() ; git++, i++) {
		DFFGeometry* geom = *git;
		QListWidgetItem* item = new QListWidgetItem(tr("Geometry %1").arg(i+1));
		ui.geometryList->addItem(item);
	}

	guiModule = new DFFGUIModule(this);

	System::getInstance()->installGUIModule(guiModule);


	connect(ui.frameTable, SIGNAL(activated(const QModelIndex&)), this,
			SLOT(frameSelected(const QModelIndex&)));
	connect(ui.geometryList, SIGNAL(currentRowChanged(int)), this, SLOT(geometrySelected(int)));
	connect(ui.materialList, SIGNAL(currentRowChanged(int)), this, SLOT(materialSelected(int)));
	connect(ui.textureList, SIGNAL(currentRowChanged(int)), this, SLOT(textureSelected(int)));
	connect(ui.geometryPartList, SIGNAL(currentRowChanged(int)), this, SLOT(geometryPartSelected(int)));

	geometryRenderWidget->updateGL();
	geometryPartRenderWidget->updateGL();

	//ui.frameList->setCurrentRow(0);
	//ui.geometryList->setCurrentRow(0);
}


DFFWidget::~DFFWidget()
{
	System::getInstance()->uninstallGUIModule(guiModule);
	delete guiModule;

	clearMaterialList();
	clearGeometryPartList();

	/*int count = ui.frameList->count();

	for (int i = 0 ; i < count ; i++) {
		delete ui.frameList->item(0);
	}*/

	int count = ui.geometryList->count();

	for (int i = 0 ; i < count ; i++) {
		delete ui.geometryList->item(0);
	}

	delete mesh;

	delete frameModel;
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


void DFFWidget::frameSelected(const QModelIndex& index)
{
	DFFFrame* frame = (DFFFrame*) index.internalPointer();

	Vector3 trans = frame->getTranslation();
	Matrix3 rot = frame->getRotation();

	ui.frameNameLabel->setText(index.data(Qt::DisplayRole).toString());
	ui.frameTranslationLabel->setText(tr("(%1, %2, %3)").arg(trans[0]).arg(trans[1]).arg(trans[2]));
	ui.frameRotationLabel->setText(tr("(%1, %2, %3) (%4, %5, %6) (%7, %8, %9)")
			.arg(rot[0]).arg(rot[1]).arg(rot[2]).arg(rot[3]).arg(rot[4]).arg(rot[5]).arg(rot[6])
			.arg(rot[7]).arg(rot[8]));

	if (frame->getParent() == NULL) {
		ui.frameParentLabel->setText(tr("None"));
	} else {
		int pidx = mesh->indexOf(frame->getParent());
		ui.frameParentLabel->setText(index.parent().data(Qt::DisplayRole).toString());
	}

	ui.frameFlagsLabel->setText(QString("%1").arg(frame->getFlags()));
}


void DFFWidget::geometrySelected(int row)
{
	printf("Geometry selected: %d\n", row);

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

	QStringList frameNames;

	DFFFrame* frame = geom->getAssociatedFrame();
	do {
		frameNames.insert(0, frame->getName());
	} while ((frame = frame->getParent())  !=  NULL);

	ui.geometryFrameLabel->setText(frameNames.join("/"));

	clearMaterialList();
	clearGeometryPartList();

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

	//ui.materialList->setCurrentRow(0);
	//ui.geometryPartList->setCurrentRow(0);

	geometryRenderWidget->renderGeometry(geom);
	geometryRenderWidget->updateGL();
}


void DFFWidget::materialSelected(int row)
{
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
	DFFGeometry* geom = mesh->getGeometry(ui.geometryList->currentRow());
	DFFGeometryPart* part = geom->getPart(row);

	ui.geometryPartNameLabel->setText(ui.geometryPartList->item(row)->text());
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


void DFFWidget::xmlDumpRequested(bool)
{
	QString filePath = QFileDialog::getSaveFileName(this, tr("Choose a destination file"), QString(),
			tr("XML Files (*.xml)"));

	if (!file.isNull()) {
		DFFXMLDumpDialog optDialog(this);

		if (optDialog.exec() == QDialog::Accepted) {
			bool frames = optDialog.isFrameDumpChecked();
			bool geoms = optDialog.isGeometryDumpChecked();
			bool parts = optDialog.isPartDumpChecked();
			bool vdata = optDialog.isVertexDataDumpChecked();
			bool mats = optDialog.isMaterialDumpChecked();
			bool indices = optDialog.isIndexDumpChecked();

			QFile file(filePath);

			if (file.open(QFile::WriteOnly | QFile::Truncate)) {
				QTextStream xml(file);

				xml << "<?xml version=\"1.0\"?>" << endl;
				xml << "<mesh>" << endl;

				if (frames) {
					xml << "  <frames count=\"" << mesh->getFrameCount() << "\">" << endl;

					DFFMesh::FrameIterator it;

					for (it = mesh->getFrameBegin() ; it != mesh->getFrameEnd() ; it++) {
						DFFFrame* frame = *it;

					}
				}
			}
		}
	}
}


void DFFWidget::xmlDumpFrame(DFFFrame* frame, QTextStream& xml, int indLevel)
{
	QString ind;

	for (int i = 0 ; i < indLevel ; i++) {
		ind << "  ";
	}

	Vector3 t = frame->getTranslation();

	xml << ind << "<frame";

	if (frame->getName()) {
		xml << " name=\"" << frame->getName() << "\"";
	}

	xml << ">" << endl;
	xml << ind << "  <translation x=\"" << t[0] << "\" y=\"" << t[1] << "\" z=\"" << t[2] << "\" />";
	xml << ind << "  <rotation "

	if (frame->getChildCount() > 0) {
		xml << ">" << endl;


		DFFFrame::ChildIterator it;
		for (it = frame->getChildBegin() ; it != frame->getChildEnd() ; it++) {
			xmlDumpFrame(*it, xml, indLevel+1);
		}

		xml << ind << "</frame>" << endl;
	} else {
		xml << " />" << endl;
	}
}

