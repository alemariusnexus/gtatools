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

#include <gtatools-gui/config.h>
#include "DFFWidget.h"
#include <QtGui/QListWidget>
#include "../../System.h"
#include <QtGui/QFileDialog>
#include "DFFXMLDumpDialog.h"
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include "DFFFormatHandler.h"
#include <QtCore/QSettings>
#include <nxcommon/strutil.h>
#include <gta/resource/texture/ManagedTextureSource.h>
#include <gta/resource/texture/StaticTextureSource.h>
#include <QtGui/QMessageBox>
#include <QtGui/QInputDialog>
#include "../../gui/GLContainerWidget.h"
#include <gtaformats/rwbs/RWBSException.h>
#include "DFFTextureSourceDialog.h"



void frameRecurse(DFFFrame* parent, int numInd)
{
	DFFFrame::ChildIterator it;

	for (it = parent->getChildBegin() ; it != parent->getChildEnd() ; it++) {
		DFFFrame* frame = *it;

		for (int i = 0 ; i < numInd ; i++) {
			printf("  ");
		}

		if (!frame->getName().isNull()) {
			printf("%s\n", frame->getName().get());
		} else {
			printf("(Unnamed)\n");
		}

		frameRecurse(frame, numInd+1);
	}
}



DFFWidget::DFFWidget(DisplayedFile* dfile, QWidget* parent)
		: QWidget(parent), dfile(dfile), mesh(NULL), frameModel(NULL), geomModel(NULL), texSource(NULL),
		  currentGeom(NULL), currentGeomPart(NULL), currentMaterial(NULL), currentTexture(NULL)
{
	ui.setupUi(this);

	File file = dfile->getFile();

	renderContainerWidget = new GLContainerWidget(ui.renderContainerWidget);
	renderContainerWidget->setWindowTitle(tr("%1 - DFF Rendering")
			.arg(file.getPath().getFileName().get()));
	ui.renderContainerWidget->layout()->addWidget(renderContainerWidget);
	renderWidget = new DFFRenderWidget(renderContainerWidget);
	renderContainerWidget->setGLWidget(renderWidget);

	updateLayoutType();

	reloadHighLevelFile();

	System* sys = System::getInstance();
	DFFGUIModule* guiModule = DFFGUIModule::getInstance();
	sys->installGUIModule(guiModule);

	connect(ui.editTexSrcButton, SIGNAL(pressed()), this, SLOT(texSrcEditRequested()));
	connect(ui.texSourceBox, SIGNAL(currentIndexChanged(int)), this, SLOT(texSrcChanged(int)));

	connect(ui.materialList, SIGNAL(currentRowChanged(int)), this, SLOT(materialSelected(int)));
	connect(ui.textureList, SIGNAL(currentRowChanged(int)), this, SLOT(textureSelected(int)));

	connect(ui.geometryFrameLabel, SIGNAL(linkActivated(const QString&)), this,
			SLOT(geometryFrameLinkActivated(const QString&)));
	connect(ui.geometryPartMaterialLabel, SIGNAL(linkActivated(const QString&)), this,
			SLOT(geometryPartMaterialLinkActivated(const QString&)));

	connect(ui.textureOpenButton, SIGNAL(pressed()), this, SLOT(textureOpenRequested()));


	connect(guiModule, SIGNAL(dumpRequested()), this, SLOT(xmlDumpRequested()));
	connect(System::getInstance(), SIGNAL(configurationChanged()), this, SLOT(updateLayoutType()));

	connect(dfile, SIGNAL(saved()), this, SLOT(reloadHighLevelFile()));
}


DFFWidget::~DFFWidget()
{
	DFFGUIModule::getInstance()->disconnect(this);
	System::getInstance()->uninstallGUIModule(DFFGUIModule::getInstance());

	clearMaterialList();

	if (frameModel)
		delete frameModel;
	if (geomModel)
		delete geomModel;
	if (mesh)
		delete mesh;
}


void DFFWidget::updateLayoutType()
{
	QSettings settings;

	bool compact = settings.value("gui/compact_mode", false).toBool();
}


void DFFWidget::reloadHighLevelFile()
{
	setCurrentGeometry(NULL);

	if (frameModel)
		delete frameModel;
	if (geomModel)
		delete geomModel;
	if (mesh)
		delete mesh;

	DFFLoader dff;

	File file = dfile->getFile();

	try {
		mesh = dff.loadMesh(file);
		setEnabled(true);
	} catch (DFFException ex) {
		System::getInstance()->log(LogEntry::error(QString(tr("Error opening the DFF file: %1"))
				.arg(ex.getMessage().get()), &ex));
		mesh = NULL;
		setEnabled(false);
		//ui.hlWidget->setEnabled(false);
		//ui.tabWidget->setTabEnabled(ui.tabWidget->indexOf(ui.hlWidget), false);
	}

	// Load the frame tree
	frameModel = new DFFFrameItemModel(mesh);
	ui.frameTree->setModel(frameModel);

	connect(ui.frameTree->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
			this, SLOT(frameSelected(const QModelIndex&, const QModelIndex&)));

	System* sys = System::getInstance();

	DFFMesh::GeometryIterator git;

	if (mesh) {
		geomModel = new DFFGeometryItemModel(mesh);
		ui.geometryTree->setModel(geomModel);

		renderWidget->displayMesh(mesh);

		connect(ui.geometryTree->selectionModel(), SIGNAL(currentChanged(const QModelIndex&, const QModelIndex&)),
				this, SLOT(geometryTreeItemSelected(const QModelIndex&, const QModelIndex&)));

		connect(geomModel, SIGNAL(geometryDisplayStateChanged(DFFGeometry*, bool)), this,
				SLOT(geometryDisplayStateChanged(DFFGeometry*, bool)));
		connect(geomModel, SIGNAL(geometryPartDisplayStateChanged(DFFGeometryPart*, bool)), this,
				SLOT(geometryPartDisplayStateChanged(DFFGeometryPart*, bool)));

		// Search for the mesh texture
		char* meshName = new char[file.getPath().getFileName().length() + 1];
		strtolower(meshName, file.getPath().getFileName().get());
		meshName[strlen(meshName)-4] = '\0';

		SystemQuery query("FindMeshTextures");
		query["meshName"] = meshName;

		QList<SystemQueryResult> results = sys->sendSystemQuery(query);

		QStringList texes;

		for (unsigned int i = 0 ; i < results.size() ; i++) {
			SystemQueryResult res = results[i];
			QStringList subTexes = res["textures"].toStringList();

			texes.append(subTexes);
		}

		for (QStringList::iterator it = texes.begin() ; it != texes.end() ; it++) {
			QString tex = *it;
			ui.texSourceBox->addItem(tex, false);
		}

		if (texes.size() > 0)
			texSrcChanged(0);

		delete[] meshName;
	}
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


void DFFWidget::setDisplayedFrame(DFFFrame* frame)
{
	if (frame) {
		/*Vector3 trans = frame->getTranslation();
		Matrix3 rot = frame->getRotation();
		const float* arot = rot.toArray();*/



		/*ui.frameTranslationLabel->setText(tr("(%1, %2, %3)").arg(trans[0], 0, 'f').arg(trans[1], 0, 'f')
				.arg(trans[2], 0, 'f'));
		ui.frameRotationLabel->setText(tr("(%1, %2, %3) (%4, %5, %6) (%7, %8, %9)")
				.arg(arot[0], -10, 'f').arg(arot[1], -10, 'f').arg(arot[2], -10, 'f').arg(arot[3], -10, 'f')
				.arg(arot[4], -10, 'f').arg(arot[5], -10, 'f').arg(arot[6], -10, 'f').arg(arot[7], -10, 'f')
				.arg(arot[8], -10, 'f'));*/

		Matrix4 mm = frame->getModelMatrix();
		const float* amm = mm.toArray();

		ui.frameModelMatrixLabel->setText(tr("(%1, %2, %3, %4) (%5, %6, %7, %8) (%9, %10, %11, %12), "
				"(%13, %14, %15, %16)").arg(amm[0]).arg(amm[1]).arg(amm[2]).arg(amm[3]).arg(amm[4])
				.arg(amm[5]).arg(amm[6]).arg(amm[7]).arg(amm[8]).arg(amm[9]).arg(amm[10]).arg(amm[11])
				.arg(amm[12]).arg(amm[13]).arg(amm[14]).arg(amm[15]));

		ui.frameFlagsLabel->setText(QString("%1b").arg(frame->getFlags(), 0, 2));

		DFFBone* bone = frame->getBone();

		if (bone) {
			ui.frameBoneIndexLabel->setText(QString("%1").arg(bone->getIndex()));
			ui.frameBoneNumberLabel->setText(QString("%1").arg(bone->getNumber()));

			QString boneType;
			switch (bone->getType()) {
			case DFFBone::Deformable:
				boneType = tr("Deformable");
				break;
			case DFFBone::Nub:
				boneType = tr("Nub");
				break;
			case DFFBone::Rigid:
				boneType = tr("Rigid");
				break;
			case DFFBone::Unknown1:
				boneType = tr("Unknown1");
				break;
			default:
				boneType = tr("[UNKNOWN: %1]").arg(bone->getType());
				break;
			}

			ui.frameBoneTypeLabel->setText(boneType);
		} else {
			ui.frameBoneIndexLabel->setText("-");
			ui.frameBoneNumberLabel->setText("-");
			ui.frameBoneTypeLabel->setText("-");
		}
	} else {
		ui.frameModelMatrixLabel->setText("-");
		ui.frameFlagsLabel->setText("-");

		ui.frameBoneIndexLabel->setText("-");
		ui.frameBoneNumberLabel->setText("-");
		ui.frameBoneTypeLabel->setText("-");
	}
}


void DFFWidget::setCurrentGeometry(DFFGeometry* geom)
{
	currentGeom = geom;
	setCurrentGeometryPart(NULL);
	setCurrentMaterial(NULL);

	if (geom) {
		ui.geometryFaceFormatLabel->setText(geom->isTriangleStripFormat()
				? tr("Triangle Strips") : tr("Triangle List"));

		QStringList vdata;

		if (geom->getVertices())
			vdata << "V";
		if (geom->getNormals())
			vdata << "N";
		if (geom->getUVSetCount() > 0)
			vdata << QString("TC (%1)").arg(geom->getUVSetCount());
		if (geom->getVertexColors())
			vdata << "C";
		if (geom->isDynamicLightingEnabled())
			vdata << "DL";

		ui.geometryVdataLabel->setText(vdata.join(", "));
		ui.geometryFlagsLabel->setText(QString("%1b").arg(geom->getFlags(), 0, 2));
		ui.geometryVertexCountLabel->setText(QString("%1").arg(geom->getVertexCount()));
		ui.geometryFrameCountLabel->setText(QString("%1").arg(geom->getFrameCount()));
		ui.geometryAmbientLightLabel->setText(QString("%1").arg(geom->getAmbientLight()));
		ui.geometryDiffuseLightLabel->setText(QString("%1").arg(geom->getDiffuseLight()));
		ui.geometrySpecularLightLabel->setText(QString("%1").arg(geom->getSpecularLight()));

		const DFFBoundingSphere* bounds = geom->getBounds();
		ui.geometryBoundsLabel->setText(QString(tr("(%1, %2, %3  :  %4)"))
				.arg(bounds->x).arg(bounds->y).arg(bounds->z).arg(bounds->radius));

		DFFFrame* frame = geom->getAssociatedFrame();
		QString frameName;

		if (frame)
			frameName = frame->getName().get();
		else
			frameName = QString(tr("Unnamed %1")).arg(frame->getParent()->indexOf(frame) + 1);

		ui.geometryFrameLabel->setText(QString("<a href=\"dummy\">%1</a>").arg(frameName));

		clearMaterialList();

		DFFGeometry::MaterialIterator it;
		int i = 0;
		for (it = geom->getMaterialBegin() ; it != geom->getMaterialEnd() ; it++, i++) {
			DFFMaterial* mat = *it;

			ui.materialList->addItem(QString(tr("Material %1")).arg(i+1));
		}
	} else {
		ui.geometryFaceFormatLabel->setText("-");
		ui.geometryVdataLabel->setText("-");
		ui.geometryFlagsLabel->setText("-");
		ui.geometryVertexCountLabel->setText("-");
		ui.geometryFrameCountLabel->setText("-");
		ui.geometryAmbientLightLabel->setText("-");
		ui.geometryDiffuseLightLabel->setText("-");
		ui.geometrySpecularLightLabel->setText("-");
		ui.geometryBoundsLabel->setText("-");
		ui.geometryFrameLabel->setText("-");

		clearMaterialList();
	}
}


void DFFWidget::setCurrentGeometryPart(DFFGeometryPart* part)
{
	if (part) {
		DFFGeometry* geom = part->getGeometry();
		setCurrentGeometry(geom);

		currentGeomPart = part;

		ui.geometryPartIndexCountLabel->setText(QString("%1").arg(part->getIndexCount()));

		if (part->getMaterial()) {
			ui.geometryPartMaterialLabel->setText(QString("<a href=\"dummy\">Material %1</a>")
					.arg(geom->indexOf(part->getMaterial()) + 1));
		} else {
			ui.geometryPartMaterialLabel->setText(tr("None"));
		}
	} else {
		currentGeomPart = part;

		ui.geometryPartIndexCountLabel->setText("-");
		ui.geometryPartMaterialLabel->setText("-");
	}
}


void DFFWidget::setCurrentMaterial(DFFMaterial* mat)
{
	currentMaterial = mat;
	setCurrentTexture(NULL);

	if (mat) {
		uint8_t r, g, b, a;
		mat->getColor(r, g, b, a);
		ui.materialColorLabel->setText(tr("(%1, %2, %3, %4)").arg(r).arg(g).arg(b).arg(a));

		clearTextureList();

		DFFMaterial::TextureIterator it;
		int i = 0;
		for (it = mat->getTextureBegin() ; it != mat->getTextureEnd() ; it++, i++) {
			DFFTexture* tex = *it;

			QString texName = QString("Texture %1").arg(i+1);

			if (!tex->getDiffuseName().isNull()) {
				texName += QString(" [%1]").arg(tex->getDiffuseName().get());
			}

			ui.textureList->addItem(texName);
		}
	} else {
		ui.materialColorLabel->setText("-");

		clearTextureList();
	}
}


void DFFWidget::setCurrentTexture(DFFTexture* tex)
{
	currentTexture = tex;

	if (tex) {
		ui.textureOpenButton->setEnabled(true);
		ui.textureDiffuseNameLabel->setText(tex->getDiffuseName().get());
		ui.textureAlphaNameLabel->setText(!tex->getAlphaName().isNull() ? tex->getAlphaName().get() : "-");
		ui.textureFilterFlagsLabel->setText(QString("%1b").arg(tex->getFilterModeFlags(), 0, 2));
	} else {
		ui.textureOpenButton->setEnabled(false);
		ui.textureDiffuseNameLabel->setText("-");
		ui.textureAlphaNameLabel->setText("-");
		ui.textureFilterFlagsLabel->setText("-");
	}
}


void DFFWidget::textureOpenRequested()
{
	const char* dname = currentTexture->getDiffuseName().get();

	SystemQuery query("FindAndOpenTexture");
	query["texture"] = QString(dname);
	System::getInstance()->sendSystemQuery(query);
}


void DFFWidget::texSrcEditRequested()
{
	DFFTextureSourceDialog dlg(this);

	for (int i = 0 ; i < ui.texSourceBox->count() ; i++) {
		QString source = ui.texSourceBox->itemText(i);
		bool path = ui.texSourceBox->itemData(i).toBool();
		dlg.addTextureSource(source, path);
	}

	if (dlg.exec() == QDialog::Accepted) {
		ui.texSourceBox->clear();

		for (int i = 0 ; i < dlg.getTextureSourceCount() ; i++) {
			bool path;
			QString source = dlg.getTextureSource(i, path);
			ui.texSourceBox->addItem(source, path);
		}
	}
}


void DFFWidget::texSrcChanged(int index)
{
	if (index == -1) {
		renderWidget->setTextureSource(NULL);
		return;
	}

	QString source = ui.texSourceBox->itemText(index);
	bool path = ui.texSourceBox->itemData(index).toBool();

	TextureSource* texSrc;

	if (path) {
		File file(source.toLocal8Bit().constData());

		if (file.exists()) {
			TXDArchive txd(file);
			texSrc = new StaticTextureSource(&txd);
		}
	} else {
		texSrc = new ManagedTextureSource(source.toAscii().constData());
	}

	renderWidget->setTextureSource(texSrc);
}


void DFFWidget::geometryFrameLinkActivated(const QString& link)
{
	DFFFrame* frame = currentGeom->getAssociatedFrame();
	QModelIndex idx = frameModel->getFrameIndex(frame);
	ui.mainTabber->setCurrentWidget(ui.frameTabWidget);
	ui.frameTree->setCurrentIndex(idx);
}


void DFFWidget::geometryPartMaterialLinkActivated(const QString& link)
{
	DFFMaterial* mat = currentGeomPart->getMaterial();
	int idx = currentGeom->indexOf(mat);
	ui.geometryTabber->setCurrentWidget(ui.geometryMaterialTabWidget);
	ui.materialList->setCurrentRow(idx);
}


void DFFWidget::frameSelected(const QModelIndex& index, const QModelIndex& previous)
{
	DFFFrame* frame = (DFFFrame*) index.internalPointer();
	setDisplayedFrame(frame);
}


void DFFWidget::geometryTreeItemSelected(const QModelIndex& index, const QModelIndex& previous)
{
	DFFGeometryItemModel::Item* item = (DFFGeometryItemModel::Item*) index.internalPointer();

	if (item->isPart()) {
		DFFGeometryItemModel::InternalDFFGeometryPart* ipart
				= (DFFGeometryItemModel::InternalDFFGeometryPart*) item;
		DFFGeometryPart* part = ipart->getPart();
		setCurrentGeometry(part->getGeometry());
		setCurrentGeometryPart(part);
	} else {
		DFFGeometryItemModel::InternalDFFGeometry* igeom = (DFFGeometryItemModel::InternalDFFGeometry*) item;
		DFFGeometry* geom = igeom->getGeometry();
		setCurrentGeometry(geom);
	}
}


void DFFWidget::materialSelected(int index)
{
	DFFMaterial* mat = currentGeom->getMaterial(index);
	setCurrentMaterial(mat);
}


void DFFWidget::textureSelected(int index)
{
	DFFTexture* tex = currentMaterial->getTexture(index);
	setCurrentTexture(tex);
}


void DFFWidget::geometryDisplayStateChanged(DFFGeometry* geom, bool displayed)
{
	renderWidget->setGeometryEnabled(geom, displayed);

	renderWidget->updateGL();
}


void DFFWidget::geometryPartDisplayStateChanged(DFFGeometryPart* part, bool displayed)
{
	/*if (renderWidget->isGeometryRendered(part->getGeometry())) {
		QLinkedList<const DFFGeometryPart*> displayedParts;
		geomModel->getDisplayedGeometryParts(part->getGeometry(), displayedParts);
		renderWidget->setGeometryParts(part->getGeometry(), displayedParts);
		renderWidget->updateGL();
	}*/

	renderWidget->setGeometryPartEnabled(part, displayed);
	renderWidget->updateGL();
}


void DFFWidget::xmlDumpRequested()
{
	if (mesh)
		DFFFormatHandler::getInstance()->xmlDumpDialog(*mesh, this);
}
