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

#include "DFFRenderWidget.h"
#include <cstdio>
#include "../../Profile.h"
#include "../../System.h"
#include "../../ProfileManager.h"
#include <gtaformats/util/File.h>
#include <gtaformats/util/thread/Mutex.h>
#include <gta/GLException.h>
#include <QtCore/QFile>
#include <QtGui/QMessageBox>
#include <gta/resource/mesh/StaticMeshPointer.h>
#include <gta/resource/mesh/Submesh.h>
#include <gta/StaticMapItemDefinition.h>
#include <gta/scene/Scene.h>
#include <gta/scene/DefaultRenderer.h>
#include <gta/scene/DepthPeelingAlgorithm.h>
#include <gta/scene/BasicTransparencyAlgorithm.h>
#include <gta/scene/visibility/PVSDatabase.h>




DFFRenderWidget::DFFRenderWidget(QWidget* parent)
		: GLBaseWidget(parent), scene(NULL), texSource(NULL)
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
	clearGeometries();

	if (scene) {
		DefaultRenderer* renderer = (DefaultRenderer*) scene->getRenderer();
		delete renderer->getTransparencyAlgorithm();
		delete renderer;
		delete scene;
	}

	if (texSource)
		delete texSource;
}


MapSceneObject* DFFRenderWidget::createSceneObject(const DFFGeometry* geom,
		QLinkedList<const DFFGeometryPart*> parts)
{
	makeCurrent();

	TextureSource* texSrc = texSource;

	if (!textured)
		texSrc = NULL;

	if (texSrc)
		texSrc = texSrc->clone();

	Mesh* mesh;

	mesh = new Mesh(*geom, false);

	for (QLinkedList<const DFFGeometryPart*>::iterator it = parts.begin() ; it != parts.end() ; it++) {
		Submesh* submesh = new Submesh(mesh, **it);
		mesh->addSubmesh(submesh);
	}

	MeshClump* clump = new MeshClump;
	clump->addMesh(mesh);

	StaticMapItemDefinition* def = new StaticMapItemDefinition(new StaticMeshPointer(clump), texSrc, NULL,
			5000.0f);
	MapSceneObject* obj = new MapSceneObject(def);

	return obj;
}


void DFFRenderWidget::addGeometry(const DFFGeometry* geom, QLinkedList<const DFFGeometryPart*> parts)
{
	makeCurrent();
	MapSceneObject* obj = createSceneObject(geom, parts);
	objs[geom] = obj;
}


void DFFRenderWidget::removeGeometry(const DFFGeometry* geom)
{
	makeCurrent();
	QMap<const DFFGeometry*, MapSceneObject*>::iterator it = objs.find(geom);

	if (it != objs.end()) {
		MapSceneObject* obj = it.value();
		MapItemDefinition* def = obj->getDefinition();
		MeshClump* clump = **def->getMeshPointer();
		delete clump;
		delete def;
		delete obj;
		objs.erase(it);
	}
}


void DFFRenderWidget::setGeometryParts(const DFFGeometry* geom,
			QLinkedList<const DFFGeometryPart*> parts)
{
	makeCurrent();
	removeGeometry(geom);
	addGeometry(geom, parts);
}


void DFFRenderWidget::clearGeometries()
{
	makeCurrent();
	while (objs.size() != 0) {
		const DFFGeometry* geom = objs.begin().key();
		removeGeometry(geom);
	}
}


void DFFRenderWidget::initializeGL()
{
	try {
		makeCurrent();

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

		GLBaseWidget::initializeGL();

		if (!scene) {
			scene = new Scene;
			PVSDatabase* pvs = new PVSDatabase;
			scene->setPVSDatabase(pvs);

			DefaultRenderer* renderer = new DefaultRenderer;
			scene->setRenderer(renderer);

			cam.move(-2.0f);
		}

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

		glEnable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		GLException::checkError();
	} catch (Exception& ex) {
		System::getInstance()->unhandeledException(ex);
	}
}


void DFFRenderWidget::resizeGL(int w, int h)
{
	GLBaseWidget::resizeGL(w, h);

	DefaultRenderer* renderer = (DefaultRenderer*) scene->getRenderer();

	Engine* engine = Engine::getInstance();
	engine->setViewportSize(getViewportWidth(), getViewportHeight());
	TransparencyAlgorithm* oldAlgo = renderer->getTransparencyAlgorithm();
	BasicTransparencyAlgorithm* btAlgo = new BasicTransparencyAlgorithm;
	renderer->setTransparencyAlgorithm(btAlgo);

	if (oldAlgo)
		delete oldAlgo;
}


void DFFRenderWidget::paintGL()
{
	try {
		GLBaseWidget::paintGL();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Engine* engine = Engine::getInstance();
		engine->setScene(scene);

		scene->clear();

		QMap<const DFFGeometry*, MapSceneObject*>::iterator it;
		for (it = objs.begin() ; it != objs.end() ; it++) {
			MapSceneObject* obj = it.value();
			scene->addSceneObject(obj);
		}

		engine->render();

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

	QMap<const DFFGeometry*, MapSceneObject*>::iterator it;
	for (it = objs.begin() ; it != objs.end() ; it++) {
		MapSceneObject* obj = it.value();
		MapItemDefinition* def = obj->getDefinition();

		if (textured) {
			if (texSource)
				def->setTextureSource(texSource->clone());
			else
				def->setTextureSource(NULL);
		} else {
			def->setTextureSource(NULL);
		}
	}
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
