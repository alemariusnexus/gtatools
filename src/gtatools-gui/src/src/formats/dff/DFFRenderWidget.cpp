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
		: QGLWidget(parent), rx(0.0f), ry(0.0f), renderList(-1), textures(true), wireframe(false),
		  currentGeometry(NULL), currentPart(NULL)
{
	connect(ProfileManager::getInstance(), SIGNAL(currentProfileChanged(Profile*, Profile*)), this,
			SLOT(currentProfileChanged(Profile*, Profile*)));
	connect(ProfileManager::getInstance()->getCurrentProfile(), SIGNAL(resourceIndexInitialized()), this,
			SLOT(currentProfileResourceIndexInitialized()));
}


DFFRenderWidget::~DFFRenderWidget()
{
}


void DFFRenderWidget::renderGeometry(DFFGeometry* geometry)
{
	if (renderList != -1) {
		glDeleteLists(renderList, 1);
	}

	Profile* profile = ProfileManager::getInstance()->getCurrentProfile();

	OpenGLResourceManager* rm = profile->getResourceManager();

	DFFOpenGLRenderer renderer(rm);

	GLuint list = glGenLists(1);

	glNewList(list, GL_COMPILE);
	renderer.renderGeometry(geometry, false);
	glEndList();

	renderList = list;

	currentGeometry = geometry;

	updateGL();
}


void DFFRenderWidget::renderGeometryPart(DFFGeometry* geometry, DFFGeometryPart* part)
{
	if (renderList != -1) {
		glDeleteLists(renderList, 1);
	}

	Profile* profile = ProfileManager::getInstance()->getCurrentProfile();

	OpenGLResourceManager* rm = profile->getResourceManager();

	DFFOpenGLRenderer renderer(rm);

	GLuint list = glGenLists(1);

	glNewList(list, GL_COMPILE);
	renderer.renderGeometryPart(geometry, part);
	glEndList();

	renderList = list;

	currentGeometry = geometry;
	currentPart = part;

	updateGL();
}


void DFFRenderWidget::initializeGL()
{
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLuint texID;
	glGenTextures(1, &texID);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBindTexture(GL_TEXTURE_2D, texID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	testTex = texID;

	unsigned char tex[128*128*4];

	for (int y = 0 ; y < 128 ; y++) {
		for (int x = 0 ; x < 128 ; x++) {
			int idx = (y*128 + x) * 4;

			if ((y+x) % 2 == 0) {
				tex[idx] = 0;
				tex[idx+1] = 0;
				tex[idx+2] = 0;
				tex[idx+3] = 255;
			} else {
				tex[idx] = 255;
				tex[idx+1] = 255;
				tex[idx+2] = 255;
				tex[idx+3] = 255;
			}
		}
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 128, 128, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);

	glGenTextures(1, &transTex);

	glBindTexture(GL_TEXTURE_2D, transTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	unsigned char tex2[3*3*4] = {
			255, 0, 0, 255,
			255, 0, 0, 255,
			255, 0, 0, 255,
			255, 0, 0, 255,
			255, 0, 0, 0,
			255, 0, 0, 255,
			255, 0, 0, 255,
			255, 0, 0, 255,
			255, 0, 0, 255
	};

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 3, 3, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex2);
	//glCullFace(GL_NONE);
}


void DFFRenderWidget::resizeGL(int w, int h)
{
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
	glColor3f(1.0f, 0.0f, 0.0f);

	glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

	if (textures) {
		glEnable(GL_TEXTURE_2D);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	/*glBindTexture(GL_TEXTURE_2D, testTex);

	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex3f(-3.0, -3.0,-2.0);

	glTexCoord2f(1, 0);
	glVertex3f(3.0, -3.0, -2.0);

	glTexCoord2f(1, 1);
	glVertex3f(3.0, 3.0, -2.0);

	glTexCoord2f(0, 1);
	glVertex3f(-3.0, 3.0, -2.0);

	glEnd();


	Profile* profile = ProfileManager::getInstance()->getCurrentProfile();
	OpenGLResourceManager* rm = profile->getResourceManager();
	DFFOpenGLRenderer renderer(rm);

	renderer.applyTexture("vehiclesteering128");

	//glBindTexture(GL_TEXTURE_2D, transTex);

	glDepthMask(GL_FALSE);

	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex3f(-1.0, -1.0, 0.0);

	glTexCoord2f(1, 0);
	glVertex3f(1.0, -1.0, 0.0);

	glTexCoord2f(1, 1);
	glVertex3f(1.0, 1.0, 0.0);

	glTexCoord2f(0, 1);
	glVertex3f(-1.0, 1.0, 0.0);

	glEnd();

	glDepthMask(GL_TRUE);*/

	if (renderList != -1) {
		glCallList(renderList);
	}

	glDisable(GL_TEXTURE_2D);

	/*Profile* profile = ProfileManager::getInstance()->getCurrentProfile();

	OpenGLResourceManager* rm = profile->getResourceManager();

	DFFOpenGLRenderer renderer(rm);

	//glEnable(GL_TEXTURE_2D);

	//renderer.applyTexture("jizzy");
	//glBindTexture(GL_TEXTURE_2D, testTex);

	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex3f(-1.0, -1.0, 0.0);

	glTexCoord2f(1, 0);
	glVertex3f(1.0, -1.0, 0.0);

	glTexCoord2f(1, 1);
	glVertex3f(1.0, 1.0, 0.0);

	glTexCoord2f(0, 1);
	glVertex3f(-1.0, 1.0, 0.0);

	glEnd();

	//glBindTexture(GL_TEXTURE_2D, testTex);

	glColor3f(1.0, 0.0, 0.0);

	glBegin(GL_QUADS);

	glTexCoord2f(0, 0);
	glVertex3f(-3.0, -3.0,-2.0);

	glTexCoord2f(1, 0);
	glVertex3f(3.0, -3.0, -2.0);

	glTexCoord2f(1, 1);
	glVertex3f(3.0, 3.0, -2.0);

	glTexCoord2f(0, 1);
	glVertex3f(-3.0, 3.0, -2.0);

	glEnd();

	//glDisable(GL_TEXTURE_2D);*/
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


void DFFRenderWidget::setShowTextures(bool textures)
{
	this->textures = textures;
	updateGL();
}


void DFFRenderWidget::setShowWireframe(bool wireframe)
{
	this->wireframe = wireframe;
	updateGL();
}


void DFFRenderWidget::currentProfileChanged(Profile* oldProfile, Profile* newProfile)
{
	disconnect(oldProfile, SIGNAL(resourceIndexInitialized()), this,
			SLOT(currentProfileResourceIndexInitialized()));
	connect(newProfile, SIGNAL(resourceIndexInitialized()), this,
			SLOT(currentProfileResourceIndexInitialized()));
}


void DFFRenderWidget::currentProfileResourceIndexInitialized()
{
	Profile* profile = ProfileManager::getInstance()->getCurrentProfile();

	if (currentGeometry) {
		if (currentPart) {
			renderGeometryPart(currentGeometry, currentPart);
		} else {
			renderGeometry(currentGeometry);
		}
	}
}
