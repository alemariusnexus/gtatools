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

#ifndef DFFWIDGET_H_
#define DFFWIDGET_H_

#include <ui_DFFWidget.h>
#include <QWidget>
#include <nxcommon/file/File.h>
#include <gtaformats/gtadff.h>
#include "DFFRenderWidget.h"
#include "DFFGUIModule.h"
#include "DFFFrameItemModel.h"
#include <qgl.h>
#include <gta/resource/texture/ManagedTextureSource.h>
#include "../../gui/GLContainerWidget.h"
#include <QCloseEvent>
#include "../../DisplayedFile.h"
#include "DFFGeometryItemModel.h"



class DFFWidget : public QWidget {
	Q_OBJECT

public:
	DFFWidget(DisplayedFile* dfile, QWidget* parent);
	~DFFWidget();

private:
	void clearMaterialList();
	void clearGeometryPartList();
	void clearTextureList();
	void xmlDumpFrame(DFFFrame* frame, QTextStream& xml, int indLevel);
	void setDisplayedFrame(DFFFrame* frame);
	void setCurrentGeometry(DFFGeometry* geom);
	void setCurrentGeometryPart(DFFGeometryPart* part);
	void setCurrentMaterial(DFFMaterial* mat);
	void setCurrentTexture(DFFTexture* tex);

private slots:
	void reloadHighLevelFile();
	void frameSelected(const QModelIndex& index, const QModelIndex& previous);
	void geometryTreeItemSelected(const QModelIndex& index, const QModelIndex& previous);
	void materialSelected(int index);
	void textureSelected(int index);
	void geometryDisplayStateChanged(DFFGeometry* geom, bool displayed);
	void geometryPartDisplayStateChanged(DFFGeometryPart* part, bool displayed);
	void geometryFrameLinkActivated(const QString& link);
	void geometryPartMaterialLinkActivated(const QString& link);
	void textureOpenRequested();
	void texSrcEditRequested();
	void texSrcChanged(int index);
	void xmlDumpRequested();
	void updateLayoutType();

private:
	Ui_DFFWidget ui;
	DisplayedFile* dfile;
	DFFMesh* mesh;
	DFFRenderWidget* renderWidget;
	DFFFrameItemModel* frameModel;
	DFFGeometryItemModel* geomModel;
	ManagedTextureSource* texSource;
	GLContainerWidget* renderContainerWidget;
	DFFGeometry* currentGeom;
	DFFGeometryPart* currentGeomPart;
	DFFMaterial* currentMaterial;
	DFFTexture* currentTexture;
};

#endif /* DFFWIDGET_H_ */
