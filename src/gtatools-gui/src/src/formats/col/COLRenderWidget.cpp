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

#include "COLRenderWidget.h"
#include <gta/render/RenderingMesh.h>
#include <gta/render/DefaultRenderingMesh.h>
#include <gta/render/RenderingSubmesh.h>
#include <gta/MeshGenerator.h>
#include <gta/resource/collision/COLMeshConverter.h>
#include <gta/GLException.h>
#include <gta/Engine.h>
#include <QtCore/QLinkedList>
#include <algorithm>
#include "../../System.h"

using std::min;
using std::max;





COLRenderWidget::COLRenderWidget(QWidget* parent)
		: GLBaseWidget(parent), renderer(NULL), model(NULL)
{
}


void COLRenderWidget::initializeGL()
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


void COLRenderWidget::resizeGL(int w, int h)
{
	GLBaseWidget::resizeGL(w, h);

	Engine* engine = Engine::getInstance();
	engine->setViewportSize(getViewportWidth(), getViewportHeight());

	if (renderer)
		delete renderer;

	renderer = new DefaultRenderer;
}


void COLRenderWidget::paintGL()
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


RenderingMesh* COLRenderWidget::createRenderingMesh(Mesh* mesh)
{
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

		Material* mat = submesh->getMaterial();

		GLuint oglTex = 0;

		uint8_t r = 255;
		uint8_t g = 255;
		uint8_t b = 255;
		uint8_t a = 255;

		if (mat) {
			mat->getColor(r, g, b, a);
		}

		RenderingSubmesh* sm = new RenderingSubmesh(rm, submesh->getIndexCount(), submesh->getIndexOffset(), oglTex);

		sm->setMaterialColor(r, g, b, a);
	}

	return rm;
}


void COLRenderWidget::releaseRenderingEntities()
{
	for (Mesh* mesh : renderingMeshes) {
		delete mesh;
	}

	renderingMeshes.clear();

	for (RenderingEntity* rent : renderingEntities) {
		delete rent;
	}

	renderingEntities.clear();
}


void COLRenderWidget::buildRenderingEntities()
{
	releaseRenderingEntities();

	if (model) {
		MeshGenerator mg;
		COLMeshConverter conv;

		const COLSphere* spheres = model->getSphereCount() != 0 ? &model->getSpheres()[0] : NULL;
		const COLBox* boxes = model->getBoxCount() != 0 ? &model->getBoxes()[0] : NULL;

		uint32_t numSpheres = model->getSphereCount();
		uint32_t numBoxes = model->getBoxCount();

		for (uint32_t i = 0 ; i < numSpheres ; i++) {
			COLSphere sphere = spheres[i];

			if (enabledSpheres.contains(i)) {
				Mesh* mesh = conv.convert(sphere);
				RenderingMesh* rmesh = createRenderingMesh(mesh);

				renderingMeshes.push_back(mesh);
				renderingEntities.push_back(rmesh);
			}
		}

		for (uint32_t i = 0 ; i < numBoxes ; i++) {
			COLBox box = boxes[i];

			if (enabledBoxes.contains(i)) {
				Mesh* mesh = conv.convert(box);
				RenderingMesh* rmesh = createRenderingMesh(mesh);

				renderingMeshes.push_back(mesh);
				renderingEntities.push_back(rmesh);
			}
		}

		if (model->getVertexCount() != 0) {
			const float* vertices = model->getVertices();
			const COLFace* faces = model->getFaceCount() != 0 ? &model->getFaces()[0] : NULL;
			const COLFaceGroup* faceGroups = model->getFaceGroupCount() != 0 ? &model->getFaceGroups()[0] : NULL;

			uint32_t numFaces = model->getFaceCount();
			uint32_t faceGroupCount = model->getFaceGroupCount();

			COLFace* displFaces = new COLFace[enabledFaces.size()];
			uint32_t faceOffs = 0;

			for (uint32_t i = 0 ; i < numFaces ; i++) {
				if (enabledFaces.contains(i)) {
					displFaces[faceOffs++] = faces[i];
				}
			}

			Mesh* mesh = conv.convert(vertices, model->getVertexCount(), displFaces, enabledFaces.size());
			RenderingMesh* rmesh = createRenderingMesh(mesh);

			renderingMeshes.push_back(mesh);
			renderingEntities.push_back(rmesh);

			delete[] displFaces;
		}
	}
}


void COLRenderWidget::displayModel(COLModel* model)
{
	releaseRenderingEntities();

	this->model = model;

	for (uint32_t i = 0 ; i < model->getSphereCount() ; i++) {
		setSphereEnabled(i, true);
	}
	for (uint32_t i = 0 ; i < model->getBoxCount() ; i++) {
		setBoxEnabled(i, true);
	}
	for (uint32_t i = 0 ; i < model->getFaceCount() ; i++) {
		setFaceEnabled(i, true);
	}
}


void COLRenderWidget::setBoxEnabled(uint32_t boxIdx, bool enabled)
{
	if (enabled) {
		enabledBoxes.insert(boxIdx);
	} else {
		enabledBoxes.remove(boxIdx);
	}
}


void COLRenderWidget::setSphereEnabled(uint32_t sphereIdx, bool enabled)
{
	if (enabled) {
		enabledSpheres.insert(sphereIdx);
	} else {
		enabledSpheres.remove(sphereIdx);
	}
}


void COLRenderWidget::setFaceEnabled(uint32_t faceIdx, bool enabled)
{
	if (faceIdx >= model->getFaceCount()) {
		return;
	}

	if (enabled) {
		enabledFaces.insert(faceIdx);
	} else {
		enabledFaces.remove(faceIdx);
	}
}
