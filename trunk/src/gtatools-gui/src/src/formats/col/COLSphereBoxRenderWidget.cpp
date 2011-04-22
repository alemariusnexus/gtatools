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
#include <gta/COLMeshConverter.h>
#include <gta/StaticMeshPointer.h>
#include <gta/NullTextureSource.h>
#include "../../System.h"



COLSphereBoxRenderWidget::COLSphereBoxRenderWidget(QWidget* parent)
		: GLBaseWidget(parent), wireframe(false)
{
}


void COLSphereBoxRenderWidget::initializeGL()
{
	GLBaseWidget::initializeGL();

#ifdef GTATOOLS_GUI_USE_OPENGL_ES
	QFile vfile(":/src/shader/vertex_col_es.glsl");
	QFile ffile(":/src/shader/fragment_col_es.glsl");
#else
	QFile vfile(":/src/shader/vertex_col.glsl");
	QFile ffile(":/src/shader/fragment_col.glsl");
#endif

	initializeShaders(vfile, ffile);

	glEnable(GL_DEPTH_TEST);
}


void COLSphereBoxRenderWidget::resizeGL(int w, int h)
{
	GLBaseWidget::resizeGL(w, h);
}


void COLSphereBoxRenderWidget::paintGL()
{
	try {
		GLBaseWidget::paintGL();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifndef GTATOOLS_GUI_USE_OPENGL_ES
		glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
#endif

		QLinkedList<ItemDefinition*>::iterator it;
	
		for (it = items.begin() ; it != items.end() ; it++) {
			ItemDefinition* item = *it;
			item->render();
		}
	} catch (Exception& ex) {
		System::getInstance()->unhandeledException(ex);
	}
}


void COLSphereBoxRenderWidget::addSphere(const COLSphere& sphere)
{
	COLMeshConverter conv;
	Mesh* mesh = conv.convert(sphere);
	ItemDefinition* item = new ItemDefinition(new StaticMeshPointer(mesh), new NullTextureSource, 5000.0f);
	items << item;
}


void COLSphereBoxRenderWidget::addBox(const COLBox& box)
{
	COLMeshConverter conv;
	Mesh* mesh = conv.convert(box);
	ItemDefinition* item = new ItemDefinition(new StaticMeshPointer(mesh), new NullTextureSource, 5000.0f);
	items << item;
}


void COLSphereBoxRenderWidget::addModel(const COLModel& model)
{
	COLMeshConverter conv;
	Mesh* mesh = conv.convert(model);
	ItemDefinition* item = new ItemDefinition(new StaticMeshPointer(mesh), new NullTextureSource, 5000.0f);
	items << item;
}


void COLSphereBoxRenderWidget::clear()
{
	QLinkedList<ItemDefinition*>::iterator it;

	for (it = items.begin() ; it != items.end() ; it++) {
		ItemDefinition* item = *it;
		delete **item->getMeshPointer();
		delete item;
	}

	items.clear();
}

