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

#include "GLBaseWidget.h"
#include <gta/Engine.h>
#include <QtCore/QPoint>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include "../System.h"




GLBaseWidget::GLBaseWidget(QWidget* parent)
		: QGLWidget(parent, System::getInstance()->getSharedGLWidget()), lastX(-1), lastY(-1),
		  moveFactor(1.0f), initialized(false)
{
	setFocusPolicy(Qt::ClickFocus);
}


void GLBaseWidget::initializeShaders(QFile& vfile, QFile& ffile)
{
	try {
		vfile.open(QFile::ReadOnly);
		QByteArray vsrc = vfile.readAll();

		ffile.open(QFile::ReadOnly);
		QByteArray fsrc = ffile.readAll();

		vertexShader = new Shader(GL_VERTEX_SHADER);
		vertexShader->loadSourceCode(vsrc.constData(), vsrc.length());
		vertexShader->compile();

		fragmentShader = new Shader(GL_FRAGMENT_SHADER);
		fragmentShader->loadSourceCode(fsrc.constData(), fsrc.length());
		fragmentShader->compile();

		program = new ShaderProgram;
		program->attachShader(vertexShader);
		program->attachShader(fragmentShader);
		program->link();
	} catch (Exception& ex) {
		System::getInstance()->unhandeledException(ex);
	}
}


void GLBaseWidget::initializeGL()
{
	makeCurrent();

	if (!initialized)
		gtaglInit();

	initialized = true;
}


void GLBaseWidget::resizeGL(int w, int h)
{
	float aspect = (float) w / (float) h;
	glViewport(0, 0, w, h);

	float l = aspect*0.035;
	float r = aspect*-0.035;
	float b = -0.035;
	float t = 0.035;
	float n = 0.05;
	float f = 3000.0;

	// glFrustum(l, r, b, t, n, f):
	pMatrix = Matrix4 (
		2*n/(r-l),	0,		0,			0,
		0,		2*n/(t-b),	0, 			0,
		(r+l)/(r-l),	(t+b)/(t-b),	(-(f+n))/(f-n),		-1,
		0,		0,		(-2*f*n)/(f-n),		0
	);
}


void GLBaseWidget::paintGL()
{
	Engine::getInstance()->setCurrentShaderProgram(program);

	Matrix4 mvpMatrix = pMatrix;
	mvpMatrix *= Matrix4::lookAt(cam.getTarget(), cam.getUp());
	mvpMatrix.translate(-cam.getPosition());

	GLint mvpMatrixUniform = program->getUniformLocation("MVPMatrix");

	if (mvpMatrixUniform != -1) {
		glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, mvpMatrix.toArray());
	}
}


void GLBaseWidget::mousePressEvent(QMouseEvent* evt)
{
	if (evt->button() == Qt::LeftButton) {
		lastX = evt->pos().x();
		lastY = evt->pos().y();
	}
}


void GLBaseWidget::mouseMoveEvent(QMouseEvent* evt)
{
	makeCurrent();

	QPoint newPos = evt->pos();

	int xo = newPos.x() - lastX;
	int yo = newPos.y() - lastY;

	lastX = newPos.x();
	lastY = newPos.y();

	if (lastX != -1  &&  lastY != -1) {
		cam.rotateHorizontal(xo*0.005f);
		cam.rotateVertical(yo*0.005f);
	}

	updateGL();
}


void GLBaseWidget::keyPressEvent(QKeyEvent* evt)
{
	bool glKey = true;

	switch (evt->key()) {
	case Qt::Key_W:
		cam.move(0.05f*moveFactor);
		break;
	case Qt::Key_S:
		cam.move(-0.05f*moveFactor);
		break;
	case Qt::Key_A:
		cam.moveSideways(-0.05f*moveFactor);
		break;
	case Qt::Key_D:
		cam.moveSideways(0.05f*moveFactor);
		break;
	case Qt::Key_Q:
		cam.moveUp(0.05f*moveFactor);
		break;
	case Qt::Key_Y:
		cam.moveUp(-0.05f*moveFactor);
		break;
	case Qt::Key_Plus:
		moveFactor *= 2.0f;
		break;
	case Qt::Key_Minus:
		moveFactor /= 2.0f;
		break;
	default:
		glKey = false;
		break;
	}

	if (glKey) {
		updateGL();
	}
}
