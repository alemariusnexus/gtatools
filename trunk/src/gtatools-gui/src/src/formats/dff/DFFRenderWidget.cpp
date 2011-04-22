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
#include <gta/StaticMeshPointer.h>
#include <gta/Submesh.h>




DFFRenderWidget::DFFRenderWidget(QWidget* parent, QGLWidget* shareWidget)
		: QGLWidget(parent, shareWidget), lastX(-1), lastY(-1), mesh(NULL), item(NULL), wireframe(false),
		  moveFactor(1.0f), renderingEnabled(true), texSource(NULL)
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
}


DFFRenderWidget::~DFFRenderWidget()
{
	if (mesh)
		delete mesh;
	if (texSource)
		delete texSource;
}


void DFFRenderWidget::renderGeometry(DFFGeometry* geometry)
{
	if (renderingEnabled) {
		makeCurrent();
		program->makeCurrent();

		if (mesh) {
			delete mesh;
		}

		Profile* profile = ProfileManager::getInstance()->getCurrentProfile();

		TextureSource* texSrc = texSource;

		if (texSrc)
			texSrc = texSrc->clone();

		mesh = new Mesh(*geometry);
		item = new ItemDefinition(new StaticMeshPointer(mesh), texSrc, 5000.0f);

		updateGL();
	}
}


void DFFRenderWidget::renderGeometryPart(DFFGeometry* geometry, DFFGeometryPart* part)
{
	if (renderingEnabled) {
		makeCurrent();
		program->makeCurrent();

		if (mesh) {
			delete mesh;
		}

		Profile* profile = ProfileManager::getInstance()->getCurrentProfile();

		TextureSource* texSrc = texSource;

		if (texSrc)
			texSrc = texSrc->clone();

		mesh = new Mesh(*geometry, false);
		mesh->addSubmesh(new Submesh(mesh, *part));
		item = new ItemDefinition(new StaticMeshPointer(mesh), texSrc, 5000.0f);

		updateGL();
	}
}


void DFFRenderWidget::initializeGL()
{
	try {
		makeCurrent();

		gtaglInit();

		if (
#ifdef GTATOOLS_GUI_USE_OPENGL_ES
			!gtaglIsVersionSupported(2, 0)
#else
			!gtaglIsVersionSupported(2, 1)
#endif
		) {
			renderingEnabled = false;
			QMessageBox::critical(this, tr("OpenGL Too Old"),
					tr("This program needs at least OpenGL version 2.1 or OpenGL ES version 2.0! "
					"Rendering will be disabled."));
		} else {
			renderingEnabled = true;
		}

		if (renderingEnabled) {
			cam.move(-2.0f);

#ifdef GTATOOLS_GUI_USE_OPENGL_ES
			QFile vfile(":/src/shader/vertex_es.glsl");
			QFile ffile(":/src/shader/fragment_es.glsl");
#else
			QFile vfile(":/src/shader/vertex.glsl");
			QFile ffile(":/src/shader/fragment.glsl");
#endif

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

			glEnable(GL_DEPTH_TEST);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			GLException::checkError();
		}
	} catch (Exception& ex) {
		System::getInstance()->unhandeledException(ex);
	}
}


void DFFRenderWidget::resizeGL(int w, int h)
{
	if (renderingEnabled) {
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
}


void DFFRenderWidget::paintGL()
{
	try {
		if (renderingEnabled) {
			Engine::getInstance()->setCurrentShaderProgram(program);

			// TODO Is this really not necessary?
			/*if (texSource) {
				glEnable(GL_TEXTURE_2D);
			} else {
				glDisable(GL_TEXTURE_2D);
			}*/

#ifndef GTATOOLS_GUI_USE_OPENGL_ES
			glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
#endif

			Matrix4 mvpMatrix = pMatrix;
			mvpMatrix *= Matrix4::lookAt(cam.getTarget(), cam.getUp());
			mvpMatrix.translate(-cam.getPosition());

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			GLint mvpMatrixUniform = program->getUniformLocation("MVPMatrix");

			if (item) {
				glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, mvpMatrix.toArray());
				item->render();
			}

			GLException::checkError();

			// TODO See above
			//glDisable(GL_TEXTURE_2D);
		}
	} catch (Exception& ex) {
		System::getInstance()->unhandeledException(ex);
	}
}


void DFFRenderWidget::mousePressEvent(QMouseEvent* evt)
{
	if (evt->button() == Qt::LeftButton) {
		lastX = evt->pos().x();
		lastY = evt->pos().y();
	}
}


void DFFRenderWidget::mouseMoveEvent(QMouseEvent* evt)
{
	if (renderingEnabled) {
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
}


void DFFRenderWidget::keyPressEvent(QKeyEvent* evt)
{
	if (renderingEnabled) {
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
}


void DFFRenderWidget::setTextureSource(TextureSource* source)
{
	if (texSource)
		delete texSource;

	texSource = source;

	if (item) {
		if (texSource)
			item->setTextureSource(texSource->clone());
		else
			item->setTextureSource(NULL);
	}

	updateGL();
}


void DFFRenderWidget::setShowWireframe(bool wireframe)
{
	if (renderingEnabled) {
		this->wireframe = wireframe;
		updateGL();
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
	if (renderingEnabled) {
		Profile* profile = ProfileManager::getInstance()->getCurrentProfile();

		updateGL();

		// TODO Reimplement
		/*if (currentGeometry) {
			if (currentPart) {
				renderGeometryPart(currentGeometry, currentPart);
			} else {
				renderGeometry(currentGeometry);
			}
		}*/
	}
}
