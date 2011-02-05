/*
 * SphereBoxRenderWidget.h
 *
 *  Created on: 30.01.2011
 *      Author: alemariusnexus
 */

#ifndef SPHEREBOXRENDERWIDGET_H_
#define SPHEREBOXRENDERWIDGET_H_

#include "../../gui/GLBaseWidget.h"
#include <GL/glew.h>
#include <gta/Shader.h>
#include <gta/ShaderProgram.h>
#include <gta/ItemDefinition.h>
#include <gtaformats/util/math/Matrix4.h>
#include <gtaformats/col/COLSphere.h>
#include <QtCore/QLinkedList>


class COLSphereBoxRenderWidget : public GLBaseWidget {
public:
	COLSphereBoxRenderWidget(QWidget* parent);
	void addSphere(const COLSphere& sphere);
	void addBox(const COLBox& box);
	void addModel(const COLModel& model);
	void clear();
	void setWireframe(bool wf) { wireframe = wf; updateGL(); }

protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();

private:
	QLinkedList<ItemDefinition*> items;
	bool wireframe;
};

#endif /* SPHEREBOXRENDERWIDGET_H_ */
