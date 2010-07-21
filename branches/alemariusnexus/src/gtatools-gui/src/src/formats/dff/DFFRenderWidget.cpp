/*
 * DFFRenderWidget.cpp
 *
 *  Created on: 20.07.2010
 *      Author: alemariusnexus
 */

#include "DFFRenderWidget.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <cstdio>
#include "../../Profile.h"
#include "../../ProfileManager.h"
#include <gtaformats/engine/DFFOpenGLRenderer.h>




DFFRenderWidget::DFFRenderWidget(QWidget* parent)
		: QGLWidget(parent), geometry(NULL), rx(0.0f), ry(0.0f)
{

}


DFFRenderWidget::~DFFRenderWidget()
{
}


void DFFRenderWidget::renderGeometry(DFFGeometry* geometry)
{
	this->geometry = geometry;

	Profile* profile = ProfileManager::getInstance()->getCurrentProfile();

	OpenGLResourceManager* rm = profile->getResourceManager();

	DFFOpenGLRenderer renderer(rm);

	GLuint list = glGenLists(1);

	glNewList(list, GL_COMPILE);
	renderer.renderGeometry(geometry, false);
	glEndList();

	renderList = list;

	updateGL();
}


void DFFRenderWidget::initializeGL()
{
	glEnable(GL_TEXTURE_2D);
	glCullFace(GL_NONE);
}


void DFFRenderWidget::resizeGL(int w, int h)
{
	//printf("Resized to %d %d\n", w, h);
	glViewport(0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70, (float) w / (float) h, 1.0, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);
}


void DFFRenderWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(1.0, 1.0, 1.0);

	/*glBegin(GL_TRIANGLE_STRIP);

	glVertex3f(-1.0, -1.0, 0.0);
	glVertex3f(1.0, -1.0, 0.0);
	glVertex3f(-1.0, 1.0, 0.0);
	glVertex3f(1.0, 1.0, 0.0);

	glEnd();*/

	if (geometry) {
		glCallList(renderList);
	}
}


void DFFRenderWidget::mousePressEvent(QMouseEvent* evt)
{
	if (evt->button() == Qt::LeftButton) {
		lastDragPos = evt->pos();
	}
}


void DFFRenderWidget::mouseMoveEvent(QMouseEvent* evt)
{
	makeCurrent();

	QPoint newPos = evt->pos();

	int xo = newPos.x() - lastDragPos.x();
	int yo = newPos.y() - lastDragPos.y();

	lastDragPos = newPos;

	rx = (float)yo*0.5f;
	ry = (float)xo*0.5f;

	glRotatef(rx, 1.0, 0.0, 0.0);
	glRotatef(ry, 0.0, 1.0, 0.0);

	updateGL();
}
