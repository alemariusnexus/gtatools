/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#include "COLSphereBoxRenderWidget.h"
#include <gta/resource/collision/COLMeshConverter.h>
#include <gta/resource/mesh/StaticMeshPointer.h>
#include <gta/resource/texture/NullTextureSource.h>
#include <gta/StaticMapItemDefinition.h>
#include <gta/scene/DefaultRenderer.h>
#include <gta/scene/DepthPeelingAlgorithm.h>
#include "../../System.h"



COLSphereBoxRenderWidget::COLSphereBoxRenderWidget(QWidget* parent)
		: GLBaseWidget(parent), scene(NULL)
{
	printf("Creating COLSphereBoxRenderWidget\n");
}


COLSphereBoxRenderWidget::~COLSphereBoxRenderWidget()
{
	clear();

	DefaultRenderer* renderer = (DefaultRenderer*) scene->getRenderer();
	delete renderer->getTransparencyAlgorithm();
	delete renderer;
	delete scene;
}


void COLSphereBoxRenderWidget::initializeGL()
{
	GLBaseWidget::initializeGL();

	return;

	if (!scene) {
		// Yes, initializeGL might be called more than once...
		scene = new Scene;

		DefaultRenderer* renderer = new DefaultRenderer;
		scene->setRenderer(renderer);
	}

	glEnable(GL_DEPTH_TEST);
}


void COLSphereBoxRenderWidget::resizeGL(int w, int h)
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


void COLSphereBoxRenderWidget::paintGL()
{
	try {
		GLBaseWidget::paintGL();

		return;

		Engine* engine = Engine::getInstance();
		engine->setScene(scene);

		scene->clear();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		QLinkedList<StaticSceneObject*>::iterator it;
	
		for (it = objs.begin() ; it != objs.end() ; it++) {
			StaticSceneObject* obj = *it;
			scene->addSceneObject(obj);
		}

		engine->render();
	} catch (Exception& ex) {
		System::getInstance()->unhandeledException(ex);
	}
}


void COLSphereBoxRenderWidget::addSphere(const COLSphere& sphere)
{
	COLMeshConverter conv;
	Mesh* mesh = conv.convert(sphere);
	MeshClump* clump = new MeshClump;
	clump->addMesh(mesh);
	MapItemDefinition* item = new StaticMapItemDefinition(new StaticMeshPointer(clump),
			new NullTextureSource, NULL, 5000.0f);
	StaticSceneObject* obj = new StaticSceneObject(item);
	objs << obj;
}


void COLSphereBoxRenderWidget::addBox(const COLBox& box)
{
	COLMeshConverter conv;
	Mesh* mesh = conv.convert(box);
	MeshClump* clump = new MeshClump;
	clump->addMesh(mesh);
	MapItemDefinition* item = new StaticMapItemDefinition(new StaticMeshPointer(clump),
			new NullTextureSource, NULL, 5000.0f);
	StaticSceneObject* obj = new StaticSceneObject(item);
	objs << obj;
}


void COLSphereBoxRenderWidget::addModel(const COLModel& model)
{
	COLMeshConverter conv;
	Mesh* mesh = conv.convert(model);
	MeshClump* clump = new MeshClump;
	clump->addMesh(mesh);
	MapItemDefinition* item = new StaticMapItemDefinition(new StaticMeshPointer(clump),
			new NullTextureSource, NULL, 5000.0f);
	StaticSceneObject* obj = new StaticSceneObject(item);
	objs << obj;
}


void COLSphereBoxRenderWidget::clear()
{
	QLinkedList<StaticSceneObject*>::iterator it;

	for (it = objs.begin() ; it != objs.end() ; it++) {
		StaticSceneObject* item = *it;
		MapItemDefinition* def = item->getDefinition();
		delete **def->getMeshPointer();
		delete def;
		delete item;
	}

	objs.clear();
}

