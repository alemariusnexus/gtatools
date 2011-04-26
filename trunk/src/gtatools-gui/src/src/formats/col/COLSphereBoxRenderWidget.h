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

#ifndef SPHEREBOXRENDERWIDGET_H_
#define SPHEREBOXRENDERWIDGET_H_

#include "../../gui/GLBaseWidget.h"
#include <gta/gl.h>
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