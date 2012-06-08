/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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
#include "../formats/FormatManager.h"
#include "../formats/FormatHandler.h"
#include <QtGui/QBoxLayout>
#include <QtCore/QList>
#include <QtCore/QSettings>
#include "ConfigWidget.h"
#include <QtGui/QMenu>
#include "../formats/txd/TXDGUIModule.h"
#include "../formats/ide/IDEGUIModule.h"
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
	sys->installGUIModule(new TXDGUIModule);
	sys->installGUIModule(IDEGUIModule::getInstance());
}


void MainWindow::initialize()
{
	System* sys = System::getInstance();

	connect(sys, SIGNAL(entityOpened(DisplayedEntity*)), this, SLOT(openEntity(DisplayedEntity*)));
	connect(sys, SIGNAL(entityClosed(DisplayedEntity*)), this, SLOT(closeEntity(DisplayedEntity*)));
	connect(sys, SIGNAL(currentEntityChanged(DisplayedEntity*, DisplayedEntity*)), this,
			SLOT(currentEntityChanged(DisplayedEntity*, DisplayedEntity*)));
	connect(sys, SIGNAL(configurationChanged()), this, SLOT(configurationChanged()));
	connect(ui.contentTabber, SIGNAL(currentChanged(int)), this, SLOT(currentEntityTabChanged(int)));
	connect(ui.contentTabber, SIGNAL(tabCloseRequested(int)), this, SLOT(entityTabClosed(int)));

	QSettings settings;

	restoreGeometry(settings.value("gui/mainwindow_geometry").toByteArray());
	restoreState(settings.value("gui/mainwindow_state").toByteArray());

	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
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


void MainWindow::openEntity(DisplayedEntity* ent)
{
	DisplayedFile* dfile = dynamic_cast<DisplayedFile*>(ent);

	if (dfile) {
		FileViewWidget* widget = new FileViewWidget(dfile);
		entityWidgets[dfile] = widget;
		ui.contentTabber->addTab(widget, dfile->getName());
	} else {
		QWidget* widget = ent->getWidget();

		if (widget) {
			entityWidgets[ent] = widget;
			ui.contentTabber->addTab(widget, ent->getName());
		}
	}

	connect(ent, SIGNAL(saved()), this, SLOT(entitySaved()));
	connect(ent, SIGNAL(changeStatusChanged()), this, SLOT(entityChangeStatusChanged()));
}


void MainWindow::currentEntityChanged(DisplayedEntity* cur, DisplayedEntity* prev)
{
	if (cur)
		ui.contentTabber->setCurrentWidget(entityWidgets[cur]);
}


void MainWindow::closeEntity(DisplayedEntity* ent)
{
	QWidget* widget = entityWidgets[ent];
	entityWidgets.remove(ent);
	ui.contentTabber->removeTab(ui.contentTabber->indexOf(widget));
	disconnect(ent, NULL, this, NULL);

	if (dynamic_cast<DisplayedFile*>(ent))
		delete widget;
}


void MainWindow::entitySaved()
{
	DisplayedEntity* dent = (DisplayedEntity*) sender();
	QWidget* widget = entityWidgets[dent];
	ui.contentTabber->setTabText(ui.contentTabber->indexOf(widget), dent->getName());
}


void MainWindow::entityChangeStatusChanged()
{
	DisplayedEntity* dent = (DisplayedEntity*) sender();
	QWidget* widget = entityWidgets[dent];
	QString text = dent->getName();

	if (dent->hasChanges())
		text += "*";

	ui.contentTabber->setTabText(ui.contentTabber->indexOf(widget), text);
}


void MainWindow::configurationChanged()
{
}


void MainWindow::taskLabelShouldAdjust()
{
}


void MainWindow::currentEntityTabChanged(int index)
{
	QMap<DisplayedEntity*, QWidget*>::iterator it;
	QWidget* widget = ui.contentTabber->widget(index);

	for (it = entityWidgets.begin() ; it != entityWidgets.end() ; it++) {
		if (widget == it.value()) {
			System* sys = System::getInstance();
			DisplayedEntity* ent = it.key();
			sys->changeCurrentEntity(ent);
			break;
		}
	}
}


void MainWindow::entityTabClosed(int index)
{
	QMap<DisplayedEntity*, QWidget*>::iterator it;
	QWidget* widget = ui.contentTabber->widget(index);

	for (it = entityWidgets.begin() ; it != entityWidgets.end() ; it++) {
		if (widget == it.value()) {
			System* sys = System::getInstance();
			sys->closeEntity(it.key());
			break;
		}
	}
}


void MainWindow::closeEvent(QCloseEvent* evt)
{
	System* sys = System::getInstance();

	if (!System::getInstance()->quit()) {
		evt->ignore();
	}
}

