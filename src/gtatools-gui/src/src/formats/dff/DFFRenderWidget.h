/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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
#include <QtCore/QLinkedList>
#include <QtCore/QMap>
#include <QtOpenGL/qgl.h>
#include <QtGui/QWidget>
#include <gtaformats/gtadff.h>
#include <gtaformats/util/math/Matrix4.h>
#include <gta/resource/mesh/Mesh.h>
#include <gta/ItemDefinition.h>
#include <gta/MapItemDefinition.h>
#include <gta/Shader.h>
#include <gta/ShaderProgram.h>
#include <gta/Camera.h>
#include <gta/resource/texture/TextureSource.h>
#include <gta/scene/objects/MapSceneObject.h>
#include <gta/scene/Scene.h>
#include <gta/render/DefaultRenderer.h>
#include <gta/render/RenderingEntity.h>
#include <gta/render/TestShaderPlugin.h>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtCore/QPoint>
#include "../../Profile.h"
#include "../../ProfileManager.h"
#include "../../gui/GLBaseWidget.h"
#include <list>

using std::list;



class DFFRenderWidget : public GLBaseWidget {
	Q_OBJECT

public:
	DFFRenderWidget(QWidget* parent);
	virtual ~DFFRenderWidget();
	/*void addGeometry(const DFFGeometry* geom, QLinkedList<const DFFGeometryPart*> parts);
	void removeGeometry(const DFFGeometry* geom);
	void setGeometryParts(const DFFGeometry* geom, QLinkedList<const DFFGeometryPart*> parts);
	void clearGeometries();
	bool isGeometryRendered(const DFFGeometry* geom) const { return objs.contains(geom); }*/
	void setTextureSource(TextureSource* source);
	TextureSource* getTextureSource() const { return texSource; }

	void displayMesh(DFFMesh* mesh);
	void setGeometryEnabled(DFFGeometry* geom, bool enabled);
	void setGeometryPartEnabled(DFFGeometryPart* part, bool enabled);

protected:
	virtual void initializeGL();
	virtual void resizeGL(int w, int h);
	virtual void paintGL();

private:
	void buildRenderingEntities();

private slots:
	void currentProfileChanged(Profile* oldProfile, Profile* newProfile);
	void currentProfileResourceIndexInitialized();
	void texturedPropertyWasChanged(bool textured);

private:
	MeshClump* displayedClump;
	QSet<Mesh*> displayedMeshes;
	QSet<Submesh*> displayedSubmeshes;
	QMap<DFFGeometry*, Mesh*> geomMeshMap;
	QMap<DFFGeometryPart*, Submesh*> partSubmeshMap;
	list<RenderingEntity*> renderingEntities;
	DefaultRenderer* renderer;
	TextureSource* texSource;
};

#endif /* DFFRENDERWIDGET_H_ */
