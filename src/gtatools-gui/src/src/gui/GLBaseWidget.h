/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#ifndef GLBASEWIDGET_H_
#define GLBASEWIDGET_H_

#include <gtatools-gui/config.h>
#include <gta/gl.h>
#include <QWidget>
#include <QGLWidget>
#include <QFile>
#include <gta/Shader.h>
#include <gta/ShaderProgram.h>
#include <gta/Camera.h>
#include <nxcommon/math/Matrix4.h>

using gta::Camera;


class GLBaseWidget : public QGLWidget {
	Q_OBJECT

public:
	GLBaseWidget(QWidget* parent);
	virtual ~GLBaseWidget();
	void setShowWireframe(bool wire) { wireframe = wire; updateGL(); }
	void setTexturingEnabled(bool enabled)
			{ textured = enabled; emit texturedPropertyChanged(enabled); updateGL(); }
	bool isShowWireframe() const { return wireframe; }
	bool isTexturingEnabled() const { return textured; }
	int getViewportWidth() const { return viewW; }
	int getViewportHeight() const { return viewH; }

protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();
	void mousePressEvent(QMouseEvent* evt);
	void mouseMoveEvent(QMouseEvent* evt);
	void keyPressEvent(QKeyEvent* evt);

signals:
	void texturedPropertyChanged(bool textured);

protected:
	bool wireframe, textured;
	Matrix4 pMatrix;
	Camera cam;
	int viewW, viewH;

private:
	int lastX, lastY;
	float moveFactor;
	bool initialized;
};

#endif /* GLBASEWIDGET_H_ */
