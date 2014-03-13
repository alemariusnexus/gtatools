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

#include "GLContainerWidget.h"
#include <QtGui/QCloseEvent>



GLContainerWidget::GLContainerWidget(QWidget* parent)
		: QWidget(parent), glWidget(NULL)
{
	ui.setupUi(this);

	toolBar = new QToolBar(ui.dockableWidget);
	ui.dockableWidget->layout()->addWidget(toolBar);

	wireframeAction = new QAction(QIcon(":/src/resource/wireframe.png"), tr("Show Wireframe"), this);
	wireframeAction->setCheckable(true);
	connect(wireframeAction, SIGNAL(toggled(bool)), this, SLOT(wireframePropertyChangedSlot(bool)));

	texturedAction = new QAction(QIcon(":/src/resource/texture.png"), tr("Show Textured"), this);
	texturedAction->setCheckable(true);
	connect(texturedAction, SIGNAL(toggled(bool)), this, SLOT(texturedPropertyChangedSlot(bool)));
	texturedAction->setChecked(true);

	undockAction = new QAction(QIcon::fromTheme("view-fullscreen",
			QIcon(":/src/resource/view-fullscreen.png")), tr("Undock Widget"), this);
	undockAction->setCheckable(true);
	connect(undockAction, SIGNAL(toggled(bool)), this, SLOT(undockPropertyChanged(bool)));

	toolBar->addAction(wireframeAction);
	toolBar->addAction(texturedAction);
	toolBar->addAction(undockAction);

	connect(ui.dockableWidget, SIGNAL(closed(QCloseEvent*)), this, SLOT(closed(QCloseEvent*)));

#ifdef GTATOOLS_GUI_USE_OPENGL_ES
	wireframeAction->setEnabled(false);
	wireframeAction->setToolTip(tr("Wireframe rendering is not available in OpenGL ES"));
#endif
}


GLContainerWidget::~GLContainerWidget()
{
	delete wireframeAction;
	delete texturedAction;
	delete undockAction;
	delete toolBar;
}


void GLContainerWidget::setGLWidget(GLBaseWidget* widget)
{
	glWidget = widget;
	ui.glContainerWidget->layout()->addWidget(widget);
}


void GLContainerWidget::undockPropertyChanged(bool undock)
{
	if (undock) {
		ui.dockableWidget->setParent(NULL);
		layout()->removeWidget(ui.dockableWidget);
		ui.dockableWidget->setAttribute(Qt::WA_QuitOnClose, false);
	} else {
		ui.dockableWidget->setParent(this);
		layout()->addWidget(ui.dockableWidget);
	}

	ui.dockableWidget->show();
}


void GLContainerWidget::wireframePropertyChangedSlot(bool wireframe)
{
	if (glWidget)
		glWidget->setShowWireframe(wireframe);

	emit wireframePropertyChanged(wireframe);
}


void GLContainerWidget::texturedPropertyChangedSlot(bool textured)
{
	if (glWidget)
		glWidget->setTexturingEnabled(textured);

	emit texturedPropertyChanged(textured);
}


void GLContainerWidget::closed(QCloseEvent* evt)
{
	undockAction->setChecked(false);
	evt->ignore();
}
