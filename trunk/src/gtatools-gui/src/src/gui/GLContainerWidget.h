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

#ifndef GLCONTAINERWIDGET_H_
#define GLCONTAINERWIDGET_H_

#include <ui_GLContainerWidget.h>
#include "GLBaseWidget.h"
#include <QtGui/QToolBar>



class GLContainerWidget : public QWidget {
	Q_OBJECT

public:
	GLContainerWidget(QWidget* parent = NULL);
	~GLContainerWidget();
	void setGLWidget(GLBaseWidget* widget);
	QWidget* getGLContainerWidget() { return ui.glContainerWidget; }
	QToolBar* getToolBar() { return toolBar; }

signals:
	void wireframePropertyChanged(bool wireframe);
	void texturedPropertyChanged(bool textured);

private slots:
	void wireframePropertyChangedSlot(bool wireframe);
	void texturedPropertyChangedSlot(bool textured);
	void undockPropertyChanged(bool undock);
	void closed(QCloseEvent* evt);

private:
	Ui_GLContainerWidget ui;
	GLBaseWidget* glWidget;
	QToolBar* toolBar;
	QAction* wireframeAction;
	QAction* texturedAction;
	QAction* undockAction;
};

#endif /* GLCONTAINERWIDGET_H_ */
