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

#ifndef DFFRENDERWIDGET_H_
#define DFFRENDERWIDGET_H_

#include <GL/glew.h>
#include <QtOpenGL/qgl.h>
#include <qwidget.h>
#include <gtaformats/gtadff.h>
#include <gtaformats/util/math/Matrix4.h>
#include <gta/StaticObjectDefinition.h>
#include <gta/Shader.h>
#include <gta/ShaderProgram.h>
#include <gta/Camera.h>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <qpoint.h>
#include "../../Profile.h"
#include "../../ProfileManager.h"



class DFFRenderWidget : public QGLWidget {
	Q_OBJECT

public:
	DFFRenderWidget(QWidget* parent, QGLWidget* shareWidget = NULL);
	virtual ~DFFRenderWidget();
	void renderGeometry(DFFGeometry* geometry);
	void renderGeometryPart(DFFGeometry* geometry, DFFGeometryPart* part);
	void setShowTextures(bool textures);
	void setShowWireframe(bool wireframe);

protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();
	void mousePressEvent(QMouseEvent* evt);
	void mouseMoveEvent(QMouseEvent* evt);
	void keyPressEvent(QKeyEvent* evt);

private slots:
	void currentProfileChanged(Profile* oldProfile, Profile* newProfile);
	void currentProfileResourceIndexInitialized();

private:
	bool textures;
	bool wireframe;
	GLuint testTex;
	GLuint transTex;
	int lastX;
	int lastY;
	DFFGeometry* currentGeometry;
	DFFGeometryPart* currentPart;
	StaticObjectDefinition* object;
	Shader* vertexShader;
	Shader* fragmentShader;
	ShaderProgram* program;
	Matrix4 pMatrix;
	Camera cam;
	GLuint dataBuffer, indexBuffer;
	float moveFactor;
	bool renderingEnabled;
};

#endif /* DFFRENDERWIDGET_H_ */
