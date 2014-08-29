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

#include "DFFRenderWidget.h"
#include <cstdio>
#include "../../Profile.h"
#include "../../System.h"
#include "../../ProfileManager.h"
#include <nxcommon/file/File.h>
#include <gta/GLException.h>
#include <QtCore/QFile>
#include <QMessageBox>
#include <gta/resource/mesh/StaticMeshPointer.h>
#include <gta/resource/mesh/Submesh.h>
#include <gta/MapItemDefinition.h>
#include <gta/scene/Scene.h>
#include <gta/render/DefaultRenderer.h>
#include <gta/render/DefaultRenderingMesh.h>
#include <gta/render/RenderingSubmesh.h>
#include <gta/resource/mesh/Mesh.h>
#include <gta/scene/visibility/PVSDatabase.h>




DFFRenderWidget::DFFRenderWidget(QWidget* parent)
		: GLBaseWidget(parent), displayedClump(NULL), renderer(NULL), texSource(NULL)
{
	setFocusPolicy(Qt::ClickFocus);

	ProfileManager* pm = ProfileManager::getInstance();

	connect(pm, SIGNAL(currentProfileChanged(Profile*, Profile*)), this,
			SLOT(currentProfileChanged(Profile*, Profile*)));

	Profile* currentProfile = pm->getCurrentProfile();
	if (currentProfile) {
		connect(currentProfile, SIGNAL(resourceIndexInitialized()), this,
				SLOT(currentProfileResourceIndexInitialized()));
	}

	connect(this, SIGNAL(texturedPropertyChanged(bool)), this, SLOT(texturedPropertyWasChanged(bool)));
}


DFFRenderWidget::~DFFRenderWidget()
{
	if (renderer)
		delete renderer;

	if (displayedClump) {
		delete displayedClump;

		for (list<RenderingEntity*>::iterator it = renderingEntities.begin() ; it != renderingEntities.end() ; it++) {
			delete *it;
		}

		renderingEntities.clear();
	}

	if (texSource)
		delete texSource;
}


void DFFRenderWidget::displayMesh(DFFMesh* mesh)
{
	makeCurrent();

	gtaglInit();

	if (displayedClump)
		delete displayedClump;

	displayedClump = new MeshClump(mesh);

	displayedMeshes.clear();
	displayedSubmeshes.clear();

	MeshClump::MeshIterator mit = displayedClump->getMeshBegin();
	DFFMesh::GeometryIterator git = mesh->getGeometryBegin();

	for (; mit != displayedClump->getMeshEnd()  &&  git != mesh->getGeometryEnd() ; mit++, git++) {
		Mesh* mesh = *mit;
		DFFGeometry* geom = *git;

		geomMeshMap.insert(geom, mesh);
		//displayedMeshes.insert(mesh);

		Mesh::SubmeshIterator sit = mesh->getSubmeshBegin();
		DFFGeometry::PartIterator pit = geom->getPartBegin();

		for (; sit != mesh->getSubmeshEnd()  &&  pit != geom->getPartEnd() ; sit++, pit++) {
			Submesh* submesh = *sit;
			DFFGeometryPart* part = *pit;

			partSubmeshMap.insert(part, submesh);
			displayedSubmeshes.insert(submesh);
		}
	}
}


void DFFRenderWidget::setGeometryEnabled(DFFGeometry* geom, bool enabled)
{
	QMap<DFFGeometry*, Mesh*>::iterator mit = geomMeshMap.find(geom);

	if (mit == geomMeshMap.end()) {
		return;
	}

	Mesh* mesh = mit.value();

	QSet<Mesh*>::iterator it = displayedMeshes.find(mesh);

	if (enabled) {
		if (it == displayedMeshes.end()) {
			displayedMeshes.insert(mesh);
		}
	} else {
		if (it != displayedMeshes.end()) {
			displayedMeshes.erase(it);
		}
	}
}


void DFFRenderWidget::setGeometryPartEnabled(DFFGeometryPart* part, bool enabled)
{
	QMap<DFFGeometryPart*, Submesh*>::iterator mit = partSubmeshMap.find(part);

	if (mit == partSubmeshMap.end()) {
		return;
	}

	Submesh* submesh = mit.value();

	QSet<Submesh*>::iterator it = displayedSubmeshes.find(submesh);

	if (enabled) {
		if (it == displayedSubmeshes.end()) {
			displayedSubmeshes.insert(submesh);
		}
	} else {
		if (it == displayedSubmeshes.end()) {
			displayedSubmeshes.erase(it);
		}
	}
}


void DFFRenderWidget::buildRenderingEntities()
{
	renderingEntities.clear();

	if (!displayedClump)
		return;

	for (MeshClump::MeshIterator it = displayedClump->getMeshBegin() ; it != displayedClump->getMeshEnd() ; it++) {
		Mesh* mesh = *it;

		if (!displayedMeshes.contains(mesh))
			continue;

		Matrix4 fModelMat = Matrix4::Identity;

		MeshFrame* frame = mesh->getFrame();

		if (frame) {
			fModelMat = fModelMat * frame->getAbsoluteModelMatrix();
		}

		RenderingPrimitiveFormat rpf;

		switch (mesh->getVertexFormat()) {
		case VertexFormatPoints:
			rpf = RenderingPrimitivePoints;
			break;
		case VertexFormatLines:
			rpf = RenderingPrimitiveLines;
			break;
		case VertexFormatTriangles:
			rpf = RenderingPrimitiveTriangles;
			break;
		case VertexFormatTriangleStrips:
			rpf = RenderingPrimitiveTriangleStrip;
			break;
		}

		//uint32_t flags = RenderingMesh::EnableShaderPluginUniformBuffers | RenderingMesh::HasTransparency;
		uint32_t flags = RenderingMesh::EnableShaderPluginUniformBuffers;

		DefaultRenderingMesh* rm = new DefaultRenderingMesh (
				rpf,
				flags,
				mesh->getVertexCount(), 0, NULL, 0,
				mesh->getDataBuffer(), mesh->getIndexBuffer(),
				mesh->getVertexOffset(), mesh->getVertexStride(),
				mesh->getSubmeshIDOffset(), mesh->getSubmeshIDStride(),
				mesh->getNormalOffset(), mesh->getNormalStride(),
				mesh->getTexCoordOffset(), mesh->getTexCoordStride(),
				mesh->getVertexColorOffset(), mesh->getVertexColorStride(),
				-1, 0,
				-1, 0
				);

		rm->setModelMatrix(fModelMat);


		for (Mesh::SubmeshIterator sit = mesh->getSubmeshBegin() ; sit != mesh->getSubmeshEnd() ; sit++) {
			Submesh* submesh = *sit;

			if (!displayedSubmeshes.contains(submesh))
				continue;

			Material* mat = submesh->getMaterial();

			GLuint oglTex = 0;

			uint8_t r = 255;
			uint8_t g = 255;
			uint8_t b = 255;
			uint8_t a = 255;

			bool texLocked = false;

			if (mat) {
				if (mat->isTextured()) {
					if (texSource) {
						Texture* tex = texSource->getTexture(mat->getTexture(), true);
						texLocked = true;
						if (tex)
							oglTex = tex->getGLTexture();
					}
				}

				mat->getColor(r, g, b, a);
			}

			RenderingSubmesh* sm = new RenderingSubmesh(rm, submesh->getIndexCount(), submesh->getIndexOffset(), oglTex);

			sm->setMaterialColor(r, g, b, a);

			if (texLocked)
				texSource->release();
		}

		renderingEntities.push_back(rm);
	}
}


void DFFRenderWidget::initializeGL()
{
	try {
		makeCurrent();

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

		GLBaseWidget::initializeGL();

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

		glDepthFunc(GL_LEQUAL);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		cam.move(-2.0f);

		GLException::checkError();
	} catch (Exception& ex) {
		System::getInstance()->unhandeledException(ex);
	}
}


void DFFRenderWidget::resizeGL(int w, int h)
{
	GLBaseWidget::resizeGL(w, h);

	Engine* engine = Engine::getInstance();
	engine->setViewportSize(getViewportWidth(), getViewportHeight());

	if (renderer)
		delete renderer;

	renderer = new DefaultRenderer;
}


void DFFRenderWidget::paintGL()
{
	try {
		renderer->setWireframeRendering(wireframe);

		GLBaseWidget::paintGL();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		buildRenderingEntities();

		renderer->enqueueForRendering(renderingEntities.begin(), renderingEntities.end());

		renderer->render();

		renderingEntities.clear();

		GLException::checkError();
	} catch (Exception& ex) {
		System::getInstance()->unhandeledException(ex);
	}
}


void DFFRenderWidget::setTextureSource(TextureSource* source)
{
	if (texSource)
		delete texSource;

	texSource = source;
}


void DFFRenderWidget::currentProfileChanged(Profile* oldProfile, Profile* newProfile)
{
	if (oldProfile) {
		disconnect(oldProfile, SIGNAL(resourceIndexInitialized()), this,
				SLOT(currentProfileResourceIndexInitialized()));
	}

	if (newProfile) {
		connect(newProfile, SIGNAL(resourceIndexInitialized()), this,
				SLOT(currentProfileResourceIndexInitialized()));
	}
}


void DFFRenderWidget::currentProfileResourceIndexInitialized()
{
	updateGL();
}


void DFFRenderWidget::texturedPropertyWasChanged(bool textured)
{
	setTextureSource(texSource ? texSource->clone() : NULL);
	updateGL();
}
