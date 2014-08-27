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

#ifndef COLRENDERWIDGET_H_
#define COLRENDERWIDGET_H_

#include <QWidget>
#include <QtCore/QSet>
#include <gtaformats/col/COLModel.h>
#include <gta/render/DefaultRenderer.h>
#include <gta/render/RenderingMesh.h>
#include <gta/resource/mesh/Mesh.h>
#include "../gui/GLBaseWidget.h"
#include <list>

using std::list;



class COLRenderWidget : public GLBaseWidget
{
	Q_OBJECT

public:
	COLRenderWidget(QWidget* parent = NULL);

	void displayModel(COLModel* model);

	void setBoxEnabled(uint32_t boxIdx, bool enabled);
	void setSphereEnabled(uint32_t sphereIdx, bool enabled);
	void setFaceEnabled(uint32_t faceIdx, bool enabled);

protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();

private:
	void buildRenderingEntities();
	void releaseRenderingEntities();
	RenderingMesh* createRenderingMesh(Mesh* mesh);

private:
	DefaultRenderer* renderer;
	list<RenderingEntity*> renderingEntities;
	list<Mesh*> renderingMeshes;

	COLModel* model;

	QSet<uint32_t> enabledSpheres;
	QSet<uint32_t> enabledBoxes;
	QSet<uint32_t> enabledFaces;
};

#endif /* COLRENDERWIDGET_H_ */
