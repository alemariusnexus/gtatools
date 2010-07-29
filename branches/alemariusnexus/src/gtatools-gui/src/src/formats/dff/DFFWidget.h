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



class DFFWidget : public QWidget {
	Q_OBJECT

public:
	DFFWidget(const File& file, QWidget* parent);
	~DFFWidget();

private:
	void clearMaterialList();
	void clearGeometryPartList();
	void clearTextureList();

private slots:
	void frameSelected(QListWidgetItem* item);
	void geometrySelected(QListWidgetItem* item);
	void materialSelected(QListWidgetItem* item);
	void textureSelected(QListWidgetItem* item);
	void geometryPartSelected(QListWidgetItem* item);
	void texturedPropertyChanged(bool textured);
	void wireframePropertyChanged(bool wireframe);

private:
	Ui_DFFWidget ui;
	DFFGUIModule* guiModule;
	DFFMesh* mesh;
	DFFRenderWidget* geometryRenderWidget;
	DFFRenderWidget* geometryPartRenderWidget;
};

#endif /* DFFWIDGET_H_ */
