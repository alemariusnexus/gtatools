/*
 * GLBaseWidget.h
 *
 *  Created on: 30.01.2011
 *      Author: alemariusnexus
 */

#ifndef GLBASEWIDGET_H_
#define GLBASEWIDGET_H_

#include <GL/glew.h>
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
