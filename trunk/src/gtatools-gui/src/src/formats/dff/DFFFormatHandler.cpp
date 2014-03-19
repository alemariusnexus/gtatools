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

#include <gta/gl.h>
#include "DFFFormatHandler.h"
#include <QtGui/QApplication>
#include "DFFWidget.h"
#include "../../System.h"
#include <QtGui/QFileDialog>
#include "DFFXMLDumpDialog.h"
#include "../../DisplayedFile.h"



DFFFormatHandler* DFFFormatHandler::getInstance()
{
	static DFFFormatHandler* inst = new DFFFormatHandler;
	return inst;
}


DFFFormatHandler::DFFFormatHandler()
{
	// Just a dummy widget to get a shared OpenGL context
	shareWidget = new QGLWidget;
}


bool DFFFormatHandler::canHandle(const EntityOpenRequest& req) const
{
	QVariant typeVar = req.getAttribute("type");

	if (typeVar.isNull())
		return false;

	if (typeVar.toString() != "file")
		return false;

	QVariant fileVar = req.getAttribute("file");

	if (fileVar.isNull())
		return false;

	File file(fileVar.toString().toLocal8Bit().constData());

	return file.guessContentType() == CONTENT_TYPE_DFF;
}


DisplayedEntity* DFFFormatHandler::openEntity(const EntityOpenRequest& request)
{
	DFFWidget* widget;

	File file(request.getAttribute("file").toString().toLocal8Bit().constData());

	DisplayedFile* dfile = new DisplayedFile(file, this, NULL);

	try {
		widget = new DFFWidget(dfile, NULL);
	} catch (Exception& ex) {
		delete dfile;
		return NULL;
	}

	dfile->setWidget(widget);

	SystemQuery query("RWBSRegisterFile");
	query.setProperty("dfile", QVariant::fromValue<void*>(dfile));
	query.setProperty("writable", true);

	System::getInstance()->sendSystemQuery(query);

	return dfile;
}


void DFFFormatHandler::xmlDumpDialog(const DFFMesh& mesh, QWidget* parent)
{
	System* sys = System::getInstance();

	DisplayedFile* dfile = dynamic_cast<DisplayedFile*>(sys->getCurrentEntity());

	QString filePath = QFileDialog::getSaveFileName(parent, tr("Choose a destination file"),
			QString(dfile->getFile().getPath().getFileName().get()).append(".xml"),
					tr("XML Files (*.xml)"));

	if (!filePath.isEmpty()) {
		DFFXMLDumpDialog optDialog(parent);

		if (optDialog.exec() == QDialog::Accepted) {
			bool frames = optDialog.isFrameDumpChecked();
			bool geoms = optDialog.isGeometryDumpChecked();
			bool parts = optDialog.isPartDumpChecked();
			bool vdata = optDialog.isVertexDataDumpChecked();
			bool mats = optDialog.isMaterialDumpChecked();
			bool bindices = optDialog.isIndexDumpChecked();

			QFile file(filePath);

			if (file.open(QFile::WriteOnly | QFile::Truncate)) {
				QTextStream xml(&file);

				xml << "<?xml version=\"1.0\"?>" << endl;
				xml << "<mesh>" << endl;

				if (frames) {
					xmlDumpFrame(mesh.getRootFrame(), xml, 1);
				}

				if (geoms) {
					xml << "  <geometries count=\"" << mesh.getGeometryCount() << "\">" << endl;

					DFFMesh::ConstGeometryIterator it;

					for (it = mesh.getGeometryBegin() ; it != mesh.getGeometryEnd() ; it++) {
						DFFGeometry* geom = *it;

						xml << "    <geometry vertexcount=\"" << geom->getVertexCount() << "\" flags=\""
								<< geom->getFlags() << "\" framecount=\"" << geom->getFrameCount()
								<< "\"";

						DFFFrame* frame = geom->getAssociatedFrame();

						if (frame) {
							QStringList framePath;

							do {
								if (!frame->getName().isNull()) {
									framePath.insert(0, frame->getName().get());
								} else {
									if (!frame->isRoot()) {
										framePath.insert(0, QString("%1")
												.arg(frame->getParent()->indexOf(frame)));
									}/* else {
										framePath.insert(0, QString("%1").arg(mesh->indexOf(frame)));
									}*/
								}
							} while ((frame = frame->getParent())  !=  NULL);

							xml << " frame=\"" << framePath.join("/") << "\"";
						}

						xml << ">" << endl;

						DFFBoundingSphere* bounds = geom->getBounds();

						xml << "      <bounds x=\"" << bounds->x << "\" y=\"" << bounds->y << "\" z=\""
								<< bounds->z << "\" r=\"" << bounds->radius << "\" />" << endl;

						if (vdata) {
							float* verts = geom->getVertices();
							float* normals = geom->getNormals();
							uint8_t* colors = geom->getVertexColors();
							float* uv = geom->getUVCoordSets();
							int32_t vc = geom->getVertexCount();
							uint8_t* bindices = geom->getBoneIndices();
							float* bweights = geom->getBoneWeights();

							xml << "      <vertices>" << endl;

							for (int32_t i = 0 ; i < geom->getVertexCount() ; i++) {
								xml << "        <vertex x=\"" << verts[i*3] << "\" y=\"" << verts[i*3+1]
										<< "\" z=\"" << verts[i*3+2] << "\" />" << endl;
							}

							xml << "      </vertices>" << endl;

							if (normals) {
								xml << "      <normals>" << endl;

								for (int32_t i = 0 ; i < geom->getVertexCount() ; i++) {
									xml << "        <normal x=\"" << normals[i*3] << "\" y=\""
											<< normals[i*3+1] << "\" z=\"" << normals[i*3+2] << "\" />"
											<< endl;
								}

								xml << "      </normals>" << endl;
							}

							if (colors) {
								xml << "      <colors>" << endl;

								for (int32_t i = 0 ; i < geom->getVertexCount() ; i++) {
									xml << "        <color r=\"" << colors[i*4] << "\" g=\"" << colors[i*4+1]
									        << "\" b=\"" << colors[i*4+2] << "\" a=\"" << colors[i*4+3]
									        << "\" />" << endl;
								}

								xml << "      </colors>" << endl;
							}

							if (uv) {
								xml << "      <texcoords>" << endl;

								for (int8_t i = 0 ; i < geom->getUVSetCount() ; i++) {
									xml << "        <set>" << endl;

									for (int32_t j = 0 ; j < geom->getVertexCount() ; j++) {
										xml << "          <texcoord u=\"" << uv[i*vc*2+j*2] << "\" v=\""
												<< uv[i*vc*2+j*2+1] << "\" />" << endl;
									}

									xml << "        </set>" << endl;
								}

								xml << "      </texcoords>" << endl;
							}

							if (bindices  ||  bweights) {
								xml << "      <skindata>" << endl;

								for (int32_t i = 0 ; i < geom->getVertexCount() ; i++) {
									xml << "        <boneassocs>" << endl;

									xml << "          <boneassoc idx=\"" << bindices[i*4] << "\" weight=\""
											<< bweights[i*4] << "\" />" << endl;
									xml << "          <boneassoc idx=\"" << bindices[i*4+1] << "\" weight=\""
											<< bweights[i*4+1] << "\" />" << endl;
									xml << "          <boneassoc idx=\"" << bindices[i*4+2] << "\" weight=\""
											<< bweights[i*4+2] << "\" />" << endl;
									xml << "          <boneassoc idx=\"" << bindices[i*4+3] << "\" weight=\""
											<< bweights[i*4+3] << "\" />" << endl;

									xml << "        </boneassocs>" << endl;
								}

								xml << "      </skindata>" << endl;
							}
						}

						if (mats) {
							xml << "      <materials count=\"" << geom->getMaterialCount() << "\">" << endl;

							DFFGeometry::MaterialIterator mit;

							for (mit = geom->getMaterialBegin() ; mit != geom->getMaterialEnd() ; mit++) {
								DFFMaterial* mat = *mit;

								uint8_t r, g, b, a;
								mat->getColor(r, g, b, a);

								xml << "        <material>" << endl;
								xml << "          <color r=\"" << r << "\" g=\"" << g << "\" b=\"" << b
										<< "\" a=\"" << a << "\" />" << endl;

								xml << "          <textures>" << endl;

								DFFMaterial::TextureIterator tit;

								for (tit = mat->getTextureBegin() ; tit != mat->getTextureEnd() ; tit++) {
									DFFTexture* tex = *tit;

									xml << "            <texture diffusename=\""
											<< tex->getDiffuseName().get() << "\" ";

									if (!tex->getAlphaName().isNull()) {
										xml << "alphaname=\"" << tex->getAlphaName().get() << "\" ";
									}

									xml << "/>" << endl;
								}

								xml << "          </textures>" << endl;

								xml << "        </material>" << endl;
							}

							xml << "      </materials>" << endl;
						}

						if (parts) {
							xml << "      <parts count=\"" << geom->getPartCount() << "\">" << endl;

							DFFGeometry::PartIterator pit;

							for (pit = geom->getPartBegin() ; pit != geom->getPartEnd() ; pit++) {
								DFFGeometryPart* part = *pit;

								xml << "        <part indexcount=\"" << part->getIndexCount()
										<< "\" material=\"" << geom->indexOf(part->getMaterial()) << "\">"
										<< endl;

								if (bindices) {
									uint32_t* indices = part->getIndices();

									xml << "          <indices count=\"" << part->getIndexCount() << "\">"
											<< endl;

									for (int32_t i = 0 ; i < part->getIndexCount() ; i++) {
										xml << "            <index v=\"" << indices[i] << "\" />" << endl;
									}

									xml << "          </indices>" << endl;
								}

								xml << "        </part>" << endl;
							}

							xml << "      </parts>" << endl;
						}

						xml << "    </geometry>" << endl;
					}

					xml << "  </geometries>" << endl;
				}

				xml << "</mesh>" << endl;

				xml.flush();

				file.close();
			}
		}
	}
}


void DFFFormatHandler::xmlDumpFrame(const DFFFrame* frame, QTextStream& xml, int indLevel)
{
	QString ind("");

	for (int i = 0 ; i < indLevel ; i++) {
		ind.append("  ");
	}

	Matrix4 mm = frame->getModelMatrix();
	const float* mma = mm.toArray();

	xml << ind << "<frame";

	if (!frame->getName().isNull()) {
		xml << " name=\"" << frame->getName().get() << "\"";
	}

	xml << " mm00=\"" << mma[0] << "\" mm01=\"" << mma[1] << "\" mm02=\"" << mma[2] << "\" mm03=\"" << mma[3]
	<< "\" mm10=\"" << mma[4] << "\" mm11=\"" << mma[5] << "\" mm12=\"" << mma[6] << "\" mm13=\"" << mma[7]
    << "\" mm20=\"" << mma[8] << "\" mm21=\"" << mma[9] << "\" mm22=\"" << mma[10] << "\" mm23=\"" << mma[11]
    << "\" mm30=\"" << mma[12] << "\" mm31=\"" << mma[13] << "\" mm32=\"" << mma[14] << "\" mm33=\"" << mma[15]
    << "\"";

	if (frame->getChildCount() > 0) {
		xml << ">" << endl;


		DFFFrame::ConstChildIterator it;
		for (it = frame->getChildBegin() ; it != frame->getChildEnd() ; it++) {
			xmlDumpFrame(*it, xml, indLevel+1);
		}

		xml << ind << "</frame>" << endl;
	} else {
		xml << " />" << endl;
	}
}
