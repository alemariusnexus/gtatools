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

#ifndef DFFWIDGET_H_
#define DFFWIDGET_H_

#include <ui_DFFWidget.h>
#include <QtGui/QWidget>
#include <gtaformats/util/File.h>
#include <gtaformats/gtadff.h>
#include "DFFRenderWidget.h"
#include "DFFGUIModule.h"
#include "DFFFrameItemModel.h"
#include <QtOpenGL/qgl.h>
#include <gta/ManagedTextureSource.h>



class DFFWidget : public QWidget {
	Q_OBJECT

public:
	DFFWidget(const File& file, QWidget* parent, QGLWidget* shareWidget);
	~DFFWidget();

private:
	void clearMaterialList();
	void clearGeometryPartList();
	void clearTextureList();
	void xmlDumpFrame(DFFFrame* frame, QTextStream& xml, int indLevel);

private slots:
	void frameSelected(const QModelIndex& index, const QModelIndex& previous);
	void geometrySelected(int row);
	void materialSelected(int row);
	void textureSelected(int row);
	void geometryPartSelected(int row);
	void texturedPropertyChanged(bool textured);
	void wireframePropertyChanged(bool wireframe);
	void xmlDumpRequested();
	void texSourceSelected(int index);
	void otherTexSourceRequested(bool);

private:
	Ui_DFFWidget ui;
	DFFMesh* mesh;
	DFFRenderWidget* geometryRenderWidget;
	DFFRenderWidget* geometryPartRenderWidget;
	DFFFrameItemModel* frameModel;
	ManagedTextureSource* texSource;
};

#endif /* DFFWIDGET_H_ */
