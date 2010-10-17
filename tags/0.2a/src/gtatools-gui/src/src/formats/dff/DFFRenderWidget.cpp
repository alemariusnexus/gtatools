/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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
 */

#include "DFFRenderWidget.h"
#include <cstdio>
#include "../../Profile.h"
#include "../../ProfileManager.h"
#include <gtaformats/util/File.h>
#include <gtaformats/util/thread/Mutex.h>
#include <gta/GLException.h>
#include <QtCore/QFile>
#include <QtGui/QMessageBox>




DFFRenderWidget::DFFRenderWidget(QWidget* parent, QGLWidget* shareWidget)
		: QGLWidget(parent, shareWidget), lastX(-1), lastY(-1), object(NULL), textures(true),
		  wireframe(false), currentGeometry(NULL), currentPart(NULL), moveFactor(1.0f)
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
	if (object) {
		delete object;
	}
}


void DFFRenderWidget::renderGeometry(DFFGeometry* geometry)
{
	if (renderingEnabled) {
		makeCurrent();
		program->makeCurrent();

		if (object) {
			delete object;
		}

		Profile* profile = ProfileManager::getInstance()->getCurrentProfile();

		ResourceManager* rm = NULL;

		if (profile) {
			rm = profile->getResourceManager();
		}

		object = new StaticObjectDefinition(geometry, NULL, rm);
		currentGeometry = geometry;

		updateGL();
	}
}


void DFFRenderWidget::renderGeometryPart(DFFGeometry* geometry, DFFGeometryPart* part)
{
	if (renderingEnabled) {
		makeCurrent();
		program->makeCurrent();

		if (object) {
			delete object;
		}

		Profile* profile = ProfileManager::getInstance()->getCurrentProfile();

		ResourceManager* rm = NULL;

		if (profile) {
			rm = profile->getResourceManager();
		}

		object = new StaticObjectDefinition(geometry, part, NULL, rm);
		currentGeometry = geometry;
		currentPart = part;

		updateGL();
	}
}


void DFFRenderWidget::initializeGL()
{
	/*if (!glewInited) {
		glewInit();
		glewInited = true;
	}*/

	makeCurrent();
	glewInit();

	if (!glewIsSupported("GL_VERSION_2_1")) {
		renderingEnabled = false;
		QMessageBox::critical(this, tr("OpenGL Too Old"),
				tr("This program needs at least OpenGL version 2.1! Rendering will be disabled."));
	} else {
		renderingEnabled = true;
	}

	if (renderingEnabled) {
		cam.move(-2.0f);

		QFile vfile(":/src/shader/vertex.glsl");
		vfile.open(QFile::ReadOnly);
		QByteArray vsrc = vfile.readAll();

		QFile ffile(":/src/shader/fragment.glsl");
		ffile.open(QFile::ReadOnly);
		QByteArray fsrc = ffile.readAll();

		vertexShader = new Shader(GL_VERTEX_SHADER);
		//vertexShader->loadSourceCode(File("/home/alemariusnexus/shader/vertex.glsl"));
		vertexShader->loadSourceCode(vsrc.constData(), vsrc.length());
		vertexShader->compile();

		fragmentShader = new Shader(GL_FRAGMENT_SHADER);
		//fragmentShader->loadSourceCode(File("/home/alemariusnexus/shader/fragment.glsl"));
		fragmentShader->loadSourceCode(fsrc.constData(), fsrc.length());
		fragmentShader->compile();

		program = new ShaderProgram;
		program->attachShader(vertexShader);
		program->attachShader(fragmentShader);
		program->link();
		program->makeCurrent();

		glEnable(GL_DEPTH_TEST);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		GLuint texID;
		glGenTextures(1, &texID);

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



		float vertices[] = {
			1.0f, -2.0f, 1.0f,
			-1.0f, -2.0f, 1.0f,
			-1.0f, -2.0f, -1.0f,
			1.0f, -2.0f, -1.0f
		};
		unsigned int indices[] = {
			0, 1, 2, 3
		};

		glGenBuffers(1, &dataBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, dataBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
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
	if (renderingEnabled) {
		program->makeCurrent();

		if (textures) {
			glEnable(GL_TEXTURE_2D);
		} else {
			glDisable(GL_TEXTURE_2D);
		}

		glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);

		Matrix4 mvpMatrix = pMatrix;
		mvpMatrix *= Matrix4::lookAt(cam.getTarget(), cam.getUp());
		mvpMatrix.translate(-cam.getPosition());

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLint mvpMatrixUniform = program->getUniformLocation("MVPMatrix");

		if (object) {
			glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, mvpMatrix.toArray());
			object->draw();
		}

		glDisable(GL_TEXTURE_2D);

		/*if (textures) {
			glEnable(GL_TEXTURE_2D);
		}

		glBindBuffer(GL_ARRAY_BUFFER, dataBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

		GLint vertexAttrib = program->getAttributeLocation("Vertex");
		GLint mvpMatrixUniform = program->getUniformLocation("MVPMatrix");
		GLint texturedUniform = program->getUniformLocation("Textured");

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Matrix4 mvpMatrix = pMatrix;
		mvpMatrix *= Matrix4::lookAt(cam.getTarget(), cam.getUp());
		mvpMatrix.translate(-cam.getPosition());

		glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, mvpMatrix.toArray());
		glUniform1i(texturedUniform, 0);

		glEnableVertexAttribArray(vertexAttrib);
		glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, (char*) 0);
		glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, (char*) 0);

		glDisable(GL_TEXTURE_2D);*/
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


void DFFRenderWidget::setShowTextures(bool textures)
{
	if (renderingEnabled) {
		this->textures = textures;
		updateGL();
	}
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

		if (currentGeometry) {
			if (currentPart) {
				renderGeometryPart(currentGeometry, currentPart);
			} else {
				renderGeometry(currentGeometry);
			}
		}
	}
}
