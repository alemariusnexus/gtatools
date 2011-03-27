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
 */

#ifndef GLBASEWIDGET_H_
#define GLBASEWIDGET_H_

#include "../config.h"
#include <gta/gl.h>
#include <QtGui/QWidget>
#include <QtOpenGL/QGLWidget>
#include <QtCore/QFile>
#include <gta/Shader.h>
#include <gta/ShaderProgram.h>
#include <gta/Camera.h>
#include <gtaformats/util/math/Matrix4.h>


class GLBaseWidget : public QGLWidget {
	Q_OBJECT

public:
	GLBaseWidget(QWidget* parent);

protected:
	void initializeShaders(QFile& vfile, QFile& ffile);
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();
	void mousePressEvent(QMouseEvent* evt);
	void mouseMoveEvent(QMouseEvent* evt);
	void keyPressEvent(QKeyEvent* evt);

private:
	Shader* vertexShader;
	Shader* fragmentShader;
	ShaderProgram* program;
	Matrix4 pMatrix;
	Camera cam;
	int lastX, lastY;
	float moveFactor;
	bool initialized;
};

#endif /* GLBASEWIDGET_H_ */
