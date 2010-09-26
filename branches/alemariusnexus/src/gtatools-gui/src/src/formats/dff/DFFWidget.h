/*
 * DFFWidget.h
 *
 *  Created on: 20.07.2010
 *      Author: alemariusnexus
 */

#ifndef DFFWIDGET_H_
#define DFFWIDGET_H_

#include <ui_DFFWidget.h>
#include <qwidget.h>
#include <gtaformats/util/File.h>
#include <gtaformats/gtadff.h>
#include "DFFRenderWidget.h"
#include "DFFGUIModule.h"
#include "DFFFrameItemModel.h"
#include <QtOpenGL/qgl.h>



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
	void xmlDumpRequested(bool);

private:
	Ui_DFFWidget ui;
	DFFGUIModule* guiModule;
	DFFMesh* mesh;
	DFFRenderWidget* geometryRenderWidget;
	DFFRenderWidget* geometryPartRenderWidget;
	DFFFrameItemModel* frameModel;
};

#endif /* DFFWIDGET_H_ */
