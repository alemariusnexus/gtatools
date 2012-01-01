/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#include "COLMeshRenderWidget.h"
#include <gta/resource/collision/COLMeshConverter.h>
#include <gta/resource/mesh/StaticMeshPointer.h>
#include <gta/resource/texture/NullTextureSource.h>
#include <gta/resource/mesh/Submesh.h>
#include <gta/scene/StaticSceneObject.h>
#include <gta/scene/Scene.h>
#include <gta/scene/DefaultRenderer.h>
#include <gta/scene/DepthPeelingAlgorithm.h>
#include <gta/StaticMapItemDefinition.h>
#include <QtGui/QMouseEvent>
#include <QtGui/QImage>
#include <QtGui/QImageWriter>
#include <fstream>
#include <gta/GLException.h>
#include "../../System.h"

using std::ofstream;



COLMeshRenderWidget::COLMeshRenderWidget(QWidget* parent)
		: GLBaseWidget(parent), scene(NULL), obj(NULL), pickObj(NULL), colors(NULL), pickedFace(-1)
{
	printf("Creating COLMeshRenderWidget\n");
}


COLMeshRenderWidget::~COLMeshRenderWidget()
{
	if (obj) {
		MapItemDefinition* def = obj->getDefinition();
		delete **def->getMeshPointer();
		delete def;
		delete obj;
	}

	if (pickObj) {
		MapItemDefinition* def = pickObj->getDefinition();
		delete **def->getMeshPointer();
		delete def;
		delete pickObj;
	}

	if (colors)
		delete[] colors;

	DefaultRenderer* renderer = (DefaultRenderer*) scene->getRenderer();
	delete renderer->getTransparencyAlgorithm();
	delete renderer;
	delete scene;
}


void COLMeshRenderWidget::render(const float* vertices, int32_t vertexCount, const COLFace* faces,
		int32_t faceCount)
{
	return;
	try {
		printf("1===>\n");
		GLBaseWidget::initializeGL();

		makeCurrent();

		COLMeshConverter conv;

		// Build visible mesh
		int modelVertexCount, modelIndexCount;
		float* modelVertices;
		uint8_t* modelColors;
		uint32_t* modelIndices;
		conv.convertVertexModel(vertices, vertexCount, faces, faceCount, modelVertexCount, modelVertices, modelColors,
				modelIndices, modelIndexCount);

		printf("Colors: %p\n", modelColors);

		Mesh* mesh = new Mesh(modelVertexCount, VertexFormatTriangles, MeshVertexColors, modelVertices, NULL,
				NULL, modelColors);
		Submesh* submesh = new Submesh(mesh, modelIndexCount, modelIndices);
		mesh->addSubmesh(submesh);

		if (colors)
			delete[] colors;

		colors = modelColors;

		MeshClump* clump = new MeshClump;
		clump->addMesh(mesh);

		if (obj) {
			MapItemDefinition* def = obj->getDefinition();
			delete **def->getMeshPointer();
			delete def;
			delete obj;
		}

		MapItemDefinition* def = new StaticMapItemDefinition(new StaticMeshPointer(clump),
				new NullTextureSource, NULL, 5000.0f);

		obj = new StaticSceneObject(def);

		// Build picking mesh
		Mesh* pickMesh = conv.convert(vertices, vertexCount, faces, faceCount);
		pickMesh->bindDataBuffer();
		int colorOffs = pickMesh->getVertexColorOffset();
		int vcount = pickMesh->getVertexCount();

		uint32_t* pickColors = new uint32_t[vcount];
		for (int32_t i = 0 ; i < faceCount ; i++) {
			pickColors[i*3] = i+1;
			pickColors[i*3+1] = i+1;
			pickColors[i*3+2] = i+1;
		}

		glBufferSubData(GL_ARRAY_BUFFER, colorOffs, vcount*4, pickColors);

		MeshClump* pickClump = new MeshClump;
		pickClump->addMesh(pickMesh);

		if (pickObj) {
			MapItemDefinition* def = pickObj->getDefinition();
			delete **def->getMeshPointer();
			delete def;
			delete pickObj;
		}

		MapItemDefinition* pickDef = new StaticMapItemDefinition(new StaticMeshPointer(pickClump),
				new NullTextureSource, NULL, 5000.0f);

		pickObj = new StaticSceneObject(pickDef);

		printf("<====\n");

		if (scene) {
			updateGL();
			printf("FUCKING UPDATED\n");
		}

		printf("HAHAHA\n");
	} catch (Exception& ex) {
		System::getInstance()->unhandeledException(ex);
	}
}

bool inited = false;


void COLMeshRenderWidget::initializeGL()
{
	printf("Initializing %p\n", this);

	//GLBaseWidget::initializeGL();
	gtaglInit();

	GLuint s = glCreateShader(GL_VERTEX_SHADER);
	printf("%s\n", glIsShader(s) == GL_TRUE ? "true" : "false");

	/*if (!inited) {
		GLuint s = glCreateShader(GL_VERTEX_SHADER);
		printf("%u: %s (%p : %s)\n", s, glIsShader(s) == GL_TRUE ? "true" : "false", context(), context()->isValid() ? "true" : "false");
		glDeleteShader(s);
		inited = true;
	} else {
		GLuint s = glCreateShader(GL_VERTEX_SHADER);
		printf("%u: %s (%p : %s)\n", s, glIsShader(s) == GL_TRUE ? "true" : "false", context(), context()->isValid() ? "true" : "false");
		inited = true;
	}*/

	/*if (scene) {
		DefaultRenderer* renderer = (DefaultRenderer*) scene->getRenderer();
		delete renderer->getTransparencyAlgorithm();
		delete renderer;
		delete scene;
	}*/

	// Yes, initializeGL might be called more than once...
	//scene = new Scene;

	//DefaultRenderer* renderer = new DefaultRenderer;
	//scene->setRenderer(renderer);

	//glEnable(GL_DEPTH_TEST);
}


void COLMeshRenderWidget::resizeGL(int w, int h)
{
	GLBaseWidget::resizeGL(w, h);

	return;

	DefaultRenderer* renderer = (DefaultRenderer*) scene->getRenderer();

	if (!renderer->getTransparencyAlgorithm()) {
		Engine* engine = Engine::getInstance();
		engine->setViewportSize(getViewportWidth(), getViewportHeight());
		DepthPeelingAlgorithm* dpAlgo = new DepthPeelingAlgorithm;
		renderer->setTransparencyAlgorithm(dpAlgo);
	}
}


void COLMeshRenderWidget::paintGL()
{
	GLBaseWidget::paintGL();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return;

	Engine* engine = Engine::getInstance();
	engine->setScene(scene);

	scene->clear();

	if (obj) {
		scene->addSceneObject(obj);
	}

	engine->render();
}


void COLMeshRenderWidget::mouseDoubleClickEvent(QMouseEvent* evt)
{
	fprintf(stderr, "WARNING: COL picking is currently unsupported due to changes in the API!");

	/*if (pickItem) {
		makeCurrent();

		int x = evt->x();
		int y = height() - evt->y();

#ifndef GTATOOLS_GUI_USE_OPENGL_ES
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

		GLBaseWidget::paintGL();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifndef GTATOOLS_GUI_USE_OPENGL_ES
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

		pickItem->render();

		int32_t pixel = 0;
		glReadPixels(x, y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel);
		pixel &= 0xFFFFFF;
		pixel--;

		setSelectedFace(pixel);

		updateGL();
	}*/
}


void COLMeshRenderWidget::setSelectedFace(int index)
{
	if (obj) {
		int prevIdx = pickedFace;


		MeshClump* clump = **obj->getDefinition()->getMeshPointer();
		Mesh* mesh = *clump->getMeshBegin();

		if (mesh->getVertexCount() != 0) {
			mesh->bindDataBuffer();
			
			int colorOffs = mesh->getVertexColorOffset();

			if (pickedFace != -1) {
				// Reset the previously picked face's color
				glBufferSubData(GL_ARRAY_BUFFER, colorOffs + pickedFace*3*4, 4, pickedFaceRealColor);
				glBufferSubData(GL_ARRAY_BUFFER, colorOffs + pickedFace*3*4 + 4, 4, pickedFaceRealColor);
				glBufferSubData(GL_ARRAY_BUFFER, colorOffs + pickedFace*3*4 + 8, 4, pickedFaceRealColor);
			}

			if (index != -1) {
				// Save the picked face's real color
				memcpy(pickedFaceRealColor, colors + index*3*4, 4);

				// And set the picked face's color to the 'picked-color'
				uint8_t pickColor[] = {255, 255, 255, 255};
				glBufferSubData(GL_ARRAY_BUFFER, colorOffs + index*3*4, 4, pickColor);
				glBufferSubData(GL_ARRAY_BUFFER, colorOffs + index*3*4 + 4, 4, pickColor);
				glBufferSubData(GL_ARRAY_BUFFER, colorOffs + index*3*4 + 8, 4, pickColor);
			}

			pickedFace = index;

			emit faceSelectionChanged(prevIdx, index);
		}
	}
}
