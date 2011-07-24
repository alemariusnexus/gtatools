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




DFFRenderWidget::DFFRenderWidget(QWidget* parent)
		: GLBaseWidget(parent), texSource(NULL)
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

	if (texSource)
		delete texSource;
}


StaticMapItemDefinition* DFFRenderWidget::createItemDefinition(const DFFGeometry* geom,
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

	StaticMapItemDefinition* item = new StaticMapItemDefinition(new StaticMeshPointer(mesh), texSrc, 5000.0f);

	return item;
}


void DFFRenderWidget::addGeometry(const DFFGeometry* geom, QLinkedList<const DFFGeometryPart*> parts)
{
	makeCurrent();
	StaticMapItemDefinition* item = createItemDefinition(geom, parts);
	items[geom] = item;
}


void DFFRenderWidget::removeGeometry(const DFFGeometry* geom)
{
	makeCurrent();
	QMap<const DFFGeometry*, ItemDefinition*>::iterator it = items.find(geom);

	if (it != items.end()) {
		ItemDefinition* item = it.value();
		Mesh* mesh = **item->getMeshPointer();
		delete item;
		delete mesh;
		items.erase(it);
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
	while (items.size() != 0) {
		const DFFGeometry* geom = items.begin().key();
		removeGeometry(geom);
	}
}


void DFFRenderWidget::initializeGL()
{
	try {
		makeCurrent();

		GLBaseWidget::initializeGL();

		cam.move(-2.0f);

#ifdef GTATOOLS_GUI_USE_OPENGL_ES
		QFile vfile(":/src/shader/vertex_es.glsl");
		QFile ffile(":/src/shader/fragment_es.glsl");
#else
		QFile vfile(":/src/shader/vertex.glsl");
		QFile ffile(":/src/shader/fragment.glsl");
#endif

		initializeShaders(vfile, ffile);

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
}


void DFFRenderWidget::paintGL()
{
	try {
		GLBaseWidget::paintGL();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		QMap<const DFFGeometry*, ItemDefinition*>::iterator it;
		for (it = items.begin() ; it != items.end() ; it++) {
			ItemDefinition* item = it.value();
			item->render();
		}

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

	QMap<const DFFGeometry*, ItemDefinition*>::iterator it;
	for (it = items.begin() ; it != items.end() ; it++) {
		ItemDefinition* item = it.value();

		if (textured) {
			if (texSource)
				item->setTextureSource(texSource->clone());
			else
				item->setTextureSource(NULL);
		} else {
			item->setTextureSource(NULL);
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
