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

#include <gtatools-gui/config.h>
#include "MainWindow.h"
#include "FileItemModel.h"
#include "../ProfileManager.h"
#include "../formats/EntityManager.h"
#include "../formats/EntityHandler.h"
#include <QtGui/QBoxLayout>
#include <QtCore/QList>
#include <QtCore/QSettings>
#include "ConfigWidget.h"
#include <QtGui/QMenu>
#include "../formats/txd/TXDGUIModule.h"
#include "../formats/ide/IDEGUIModule.h"
#include "../formats/rwbs/RWBSGUIModule.h"
#include "../System.h"
#include "DefaultGUIModule.h"
#include <QtGui/QProgressBar>
#include <QtGui/QHeaderView>
#include <QtGui/QLayout>
#include <QtGui/QTableWidget>
#include <QtCore/QTimer>
#include <QtGui/QHBoxLayout>
#include <QtGui/qdockwidget.h>


MainWindow::MainWindow()
{
	ui.setupUi(this);
	setWindowTitle(tr("ProgramBaseName"));

	taskLabel = new QLabel(ui.statusbar);
	ui.statusbar->addPermanentWidget(taskLabel);

	progressBar = new QProgressBar(ui.statusbar);
	progressBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	progressBar->setMinimumWidth(200);
	ui.statusbar->addPermanentWidget(progressBar);

	System* sys = System::getInstance();
	sys->setMainWindow(this);

	setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::North);
	setTabPosition(Qt::RightDockWidgetArea, QTabWidget::North);
	setTabPosition(Qt::BottomDockWidgetArea, QTabWidget::North);
	setTabPosition(Qt::TopDockWidgetArea, QTabWidget::North);

	sys->installGUIModule(new DefaultGUIModule);

	centralTabber = new QTabWidget(this);
	centralTabber->setTabsClosable(true);
	setCentralWidget(centralTabber);

	//setCentralWidget(new QWidget);
	//centralWidget()->hide();

	// TODO Reactivate
	sys->installGUIModule(RWBSGUIModule::getInstance());
	//sys->installGUIModule(new TXDGUIModule);
	//sys->installGUIModule(IDEGUIModule::getInstance());
}


void MainWindow::initialize()
{
	System* sys = System::getInstance();

	connect(sys, SIGNAL(entityOpened(DisplayedEntity*)), this, SLOT(openEntity(DisplayedEntity*)));
	connect(sys, SIGNAL(entityClosed(DisplayedEntity*)), this, SLOT(closeEntity(DisplayedEntity*)));
	connect(sys, SIGNAL(currentEntityChanged(DisplayedEntity*, DisplayedEntity*)), this,
			SLOT(currentEntityChanged(DisplayedEntity*, DisplayedEntity*)));
	connect(sys, SIGNAL(configurationChanged()), this, SLOT(configurationChanged()));

	connect(centralTabber, SIGNAL(tabCloseRequested(int)), this, SLOT(centralTabWidgetCloseRequested(int)));
	connect(centralTabber, SIGNAL(currentChanged(int)), this, SLOT(centralTabWidgetCurrentChanged(int)));

	QSettings settings;

	// show() must be executed before restoreGeometry(), because some stupid X11 window managers don't restore the
	// maximized state otherwise.
	restoreGeometry(settings.value("gui/mainwindow_geometry").toByteArray());

	QTimer::singleShot(500, this, SLOT(loadWindowState()));

	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
}


void MainWindow::loadWindowState()
{
	QSettings settings;
	restoreState(settings.value("gui/mainwindow_state").toByteArray());
}


MainWindow::~MainWindow()
{
	QSettings settings;
	settings.setValue("gui/mainwindow_geometry", saveGeometry());
	settings.setValue("gui/mainwindow_state", saveState());

	System* sys = System::getInstance();

	QLinkedList<GUIModule*> guiModules = sys->getInstalledGUIModules();

	for (QLinkedList<GUIModule*>::iterator it = guiModules.begin() ; it != guiModules.end() ; it++) {
		GUIModule* module = *it;
		sys->forceUninstallGUIModule(module);
	}

	while (dockWidgets.size() != 0) {
		QDockWidget* widget = dockWidgets[0];
		removeDockWidget(widget);
		delete widget;
	}

	delete taskLabel;
	delete progressBar;
}


void MainWindow::addDockWidget(Qt::DockWidgetArea area, QDockWidget* widget)
{
	QMainWindow::addDockWidget(area, widget);
	QAction* action = new QAction(widget->windowTitle(), this);

	dockWidgets << widget;
	dockViewActions << action;

	action->setData(dockWidgets.size()-1);
	action->setCheckable(true);
	action->setChecked(widget->isVisible());
	ui.menuWindows->addAction(action);
	connect(action, SIGNAL(triggered(bool)), this, SLOT(dockWidgetViewChanged(bool)));
	connect(widget, SIGNAL(visibilityChanged(bool)), this, SLOT(dockWidgetVisibilityChanged(bool)));
}


void MainWindow::removeDockWidget(QDockWidget* widget)
{
	disconnect(widget, SIGNAL(visibilityChanged(bool)), this, SLOT(dockWidgetVisibilityChanged(bool)));

	QLinkedList<QAction*>::iterator it;
	int index = dockWidgets.indexOf(widget);
	QAction* action = dockViewActions[index];
	disconnect(action, SIGNAL(triggered(bool)), this, SLOT(dockWidgetViewChanged(bool)));
	delete action;

	dockWidgets.removeAt(index);
	dockViewActions.removeAt(index);

	QMainWindow::removeDockWidget(widget);
}


void MainWindow::dockWidgetViewChanged(bool checked)
{
	QAction* action = (QAction*) sender();
	QDockWidget* widget = dockWidgets[action->data().toInt()];

	if (checked) {
		if (!widget->isVisible()) {
			widget->show();
		}
	} else {
		if (widget->isVisible()) {
			widget->close();
		}
	}
}


void MainWindow::dockWidgetVisibilityChanged(bool visible)
{
	QDockWidget* widget = (QDockWidget*) sender();
	QLinkedList<QAction*>::iterator it;
	int index = dockWidgets.indexOf(widget);
	QAction* action = dockViewActions[index];
	action->setChecked(visible);
}


void MainWindow::configurationChanged()
{
}


void MainWindow::taskLabelShouldAdjust()
{
}


void MainWindow::closeEvent(QCloseEvent* evt)
{
	System* sys = System::getInstance();

	if (!System::getInstance()->quit()) {
		evt->ignore();
	}
}


void MainWindow::openEntity(DisplayedEntity* entity)
{
	QWidget* widget = entity->getWidget();

	if (widget) {
		centralTabber->addTab(widget, entity->getName());
		entityWidgets[entity] = widget;
	}

	connect(entity, SIGNAL(hasUnsavedChangesStateChanged(bool)), this, SLOT(entityChangeStatusChanged(bool)));
}


void MainWindow::closeEntity(DisplayedEntity* entity)
{
	QMap<DisplayedEntity*, QWidget*>::iterator it = entityWidgets.find(entity);

	if (it != entityWidgets.end()) {
		QWidget* w = it.value();
		centralTabber->removeTab(centralTabber->indexOf(w));
		entityWidgets.erase(it);
	}

	disconnect(entity, SIGNAL(hasUnsavedChangesStateChanged(bool)), this, SLOT(entityChangeStatusChanged(bool)));
}


void MainWindow::currentEntityChanged(DisplayedEntity* oe, DisplayedEntity* ne)
{
	if (!ne)
		return;

	QMap<DisplayedEntity*, QWidget*>::iterator it = entityWidgets.find(ne);

	if (it != entityWidgets.end()) {
		QWidget* w = it.value();
		centralTabber->setCurrentWidget(w);
	}
}


void MainWindow::centralTabWidgetCloseRequested(int index)
{
	QWidget* w = centralTabber->widget(index);

	for (QMap<DisplayedEntity*, QWidget*>::iterator it = entityWidgets.begin() ; it != entityWidgets.end() ; it++) {
		if (it.value() == w) {
			DisplayedEntity* ent = it.key();

			System* sys = System::getInstance();

			sys->closeEntity(ent);

			break;
		}
	}
}


void MainWindow::centralTabWidgetCurrentChanged(int index)
{
	QWidget* w = centralTabber->widget(index);

	for (QMap<DisplayedEntity*, QWidget*>::iterator it = entityWidgets.begin() ; it != entityWidgets.end() ; it++) {
		if (it.value() == w) {
			DisplayedEntity* ent = it.key();

			System* sys = System::getInstance();

			sys->changeCurrentEntity(ent);

			break;
		}
	}
}


void MainWindow::entityChangeStatusChanged(bool hasChanges)
{
	DisplayedEntity* entity = (DisplayedEntity*) sender();

	QMap<DisplayedEntity*, QWidget*>::iterator it = entityWidgets.find(entity);

	if (it != entityWidgets.end()) {
		QWidget* w = it.value();

		QString text;

		if (entity->hasUnsavedChanges()) {
			text = QString("%1*").arg(entity->getName());
		} else {
			text = entity->getName();
		}

		centralTabber->setTabText(centralTabber->indexOf(w), text);
	}
}

