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

#ifndef DFFRENDERWIDGET_H_
#define DFFRENDERWIDGET_H_

#include <gtatools-gui/config.h>
#include <gta/gl.h>
#include <QtOpenGL/qgl.h>
#include <QtGui/QWidget>
#include <gtaformats/gtadff.h>
#include <gtaformats/util/math/Matrix4.h>
#include <gta/Mesh.h>
#include <gta/ItemDefinition.h>
#include <gta/Shader.h>
#include <gta/ShaderProgram.h>
#include <gta/Camera.h>
#include <gta/TextureSource.h>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtCore/QPoint>
#include "../../Profile.h"
#include "../../ProfileManager.h"



class DFFRenderWidget : public QGLWidget {
	Q_OBJECT

public:
	DFFRenderWidget(QWidget* parent, QGLWidget* shareWidget = NULL);
	virtual ~DFFRenderWidget();
	void renderGeometry(DFFGeometry* geometry);
	void renderGeometryPart(DFFGeometry* geometry, DFFGeometryPart* part);
	void setShowWireframe(bool wireframe);
	void setTextureSource(TextureSource* source);
	TextureSource* getTextureSource() const { return texSource; }

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
	bool wireframe;
	int lastX;
	int lastY;
	Mesh* mesh;
	ItemDefinition* item;
	Shader* vertexShader;
	Shader* fragmentShader;
	ShaderProgram* program;
	Matrix4 pMatrix;
	Camera cam;
	float moveFactor;
	bool renderingEnabled;
	TextureSource* texSource;

	GLint vertexAttrib, normalAttrib, texCoordAttrib, colorAttrib;
};

#endif /* DFFRENDERWIDGET_H_ */
