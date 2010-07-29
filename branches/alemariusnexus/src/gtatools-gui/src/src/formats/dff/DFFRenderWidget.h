/*
 * DFFRenderWidget.h
 *
 *  Created on: 20.07.2010
 *      Author: alemariusnexus
 */

#ifndef DFFRENDERWIDGET_H_
#define DFFRENDERWIDGET_H_

#include <QtOpenGL/qgl.h>
#include <qwidget.h>
#include <gtaformats/gtadff.h>
#include <QtGui/QMouseEvent>
#include <qpoint.h>



class DFFRenderWidget : public QGLWidget {
	Q_OBJECT

public:
	DFFRenderWidget(QWidget* parent);
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

private:
	bool textures;
	bool wireframe;
	GLuint testTex;
	GLuint transTex;
	GLuint renderList;
	QPoint lastDragPos;
	float rx;
	float ry;
};

#endif /* DFFRENDERWIDGET_H_ */
