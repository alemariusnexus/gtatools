/*
 * SphereBoxRenderWidget.cpp
 *
 *  Created on: 30.01.2011
 *      Author: alemariusnexus
 */

#include "COLSphereBoxRenderWidget.h"
#include <gta/COLMeshConverter.h>
#include <gta/StaticMeshPointer.h>
#include <gta/NullTextureSource.h>



COLSphereBoxRenderWidget::COLSphereBoxRenderWidget(QWidget* parent)
		: GLBaseWidget(parent), wireframe(false)
{
}


void COLSphereBoxRenderWidget::initializeGL()
{
	GLBaseWidget::initializeGL();

	QFile vfile(":/src/shader/vertex_col.glsl");
	QFile ffile(":/src/shader/fragment_col.glsl");

	initializeShaders(vfile, ffile);

	glEnable(GL_DEPTH_TEST);
}


void COLSphereBoxRenderWidget::resizeGL(int w, int h)
{
	GLBaseWidget::resizeGL(w, h);
}


void COLSphereBoxRenderWidget::paintGL()
{
	GLBaseWidget::paintGL();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

	QLinkedList<ItemDefinition*>::iterator it;

	for (it = items.begin() ; it != items.end() ; it++) {
		ItemDefinition* item = *it;
		item->render();
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

