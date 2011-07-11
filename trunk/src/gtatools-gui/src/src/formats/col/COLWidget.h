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


#ifndef COLWIDGET_H
#define COLWIDGET_H

#include <QtGui/QWidget>
#include "ui_COLWidget.h"
#include <gtaformats/gtacol.h>
#include <QtCore/QList>
#include "COLSphereBoxRenderWidget.h"
#include "COLMeshRenderWidget.h"
#include "../../gui/GLContainerWidget.h"

class COLWidget : public QWidget
{
    Q_OBJECT

public:
    COLWidget(const File& file, QWidget *parent = 0);
    ~COLWidget();

private:
    void updateVertexMesh();

private slots:
    void currentModelChanged(int index);
    void faceGroupsToggled(bool status);
    void faceGroupSelectionChanged();
    void currentMeshChanged(int index);
	void shadowMeshFaceSelectionChanged(int prevIdx, int idx);
	void vertexMeshFaceSelectionChanged(int prevIdx, int idx);
	void updateLayoutType();

private:
    Ui::COLWidget ui;
    QList<COLModel*> models;
    COLSphereBoxRenderWidget* sphereBoxRenderer;
    COLMeshRenderWidget* meshRenderer;
    COLMeshRenderWidget* shadowMeshRenderer;
    QList<int> vmeshFaceIndexMap;
    bool currentlyCompact;

    QTabWidget* sphereBoxTabber;
    QTabWidget* vmeshTabber;
    QTabWidget* smeshTabber;

    GLContainerWidget* sphereBoxRendererContainer;
    GLContainerWidget* meshRendererContainer;
    GLContainerWidget* shadowMeshRendererContainer;
};

#endif // COLWIDGET_H
