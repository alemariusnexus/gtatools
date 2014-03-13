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

#ifndef COLMESHRENDERWIDGET_H_
#define COLMESHRENDERWIDGET_H_

#include "../../gui/GLBaseWidget.h"
#include <gtaformats/gtacol.h>
#include <gta/MapItemDefinition.h>
#include <gta/scene/objects/MapSceneObject.h>
#include <gta/scene/Scene.h>


class COLMeshRenderWidget : public GLBaseWidget {
	Q_OBJECT

public:
	COLMeshRenderWidget(QWidget* parent);
	~COLMeshRenderWidget();
	void render(const float* vertices, int32_t vertexCount, const COLFace* faces, int32_t faceCount);
	void setSelectedFace(int faceIndex);

protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();
	void mouseDoubleClickEvent(QMouseEvent* evt);

signals:
	void faceSelectionChanged(int prevIdx, int idx);

private:
	Scene* scene;
	MapSceneObject* obj;
	MapSceneObject* pickObj;
	uint8_t* colors;
	int pickedFace;
	uint8_t pickedFaceRealColor[4];
};

#endif /* COLMESHRENDERWIDGET_H_ */
