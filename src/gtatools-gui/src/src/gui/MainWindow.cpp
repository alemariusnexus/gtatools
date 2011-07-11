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

	connect(sys, SIGNAL(fileOpened(const FileOpenRequest&, DisplayedFile*)), this,
			SLOT(openFile(const FileOpenRequest&, DisplayedFile*)));
	connect(sys, SIGNAL(fileClosed(DisplayedFile*)), this, SLOT(closeFile(DisplayedFile*)));
	connect(sys, SIGNAL(currentFileChanged(DisplayedFile*, DisplayedFile*)), this,
			SLOT(currentFileChanged(DisplayedFile*, DisplayedFile*)));
	connect(sys, SIGNAL(configurationChanged()), this, SLOT(configurationChanged()));
	connect(ui.contentTabber, SIGNAL(currentChanged(int)), this, SLOT(currentFileTabChanged(int)));
	connect(ui.contentTabber, SIGNAL(tabCloseRequested(int)), this, SLOT(fileTabClosed(int)));

	QSettings settings;

	resize(settings.value("gui/mainwindow_size", size()).toSize());
	restoreState(settings.value("gui/mainwindow_state").toByteArray());

	setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
}


MainWindow::~MainWindow()
{
	QSettings settings;
	settings.setValue("gui/mainwindow_size", size());
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


void MainWindow::openFile(const FileOpenRequest& request, DisplayedFile* file)
{
	FileViewWidget* widget = new FileViewWidget(file);
	fileWidgets[file] = widget;
	ui.contentTabber->addTab(widget, QString(file->getFile().getPath()->getFileName()));
	connect(file, SIGNAL(saved(const File&)), this, SLOT(fileSaved(const File&)));
	connect(file, SIGNAL(changeStatusChanged()), this, SLOT(fileChangeStatusChanged()));
}


void MainWindow::currentFileChanged(DisplayedFile* file, DisplayedFile* prev)
{
	if (file)
		ui.contentTabber->setCurrentWidget(fileWidgets[file]);
}


void MainWindow::closeFile(DisplayedFile* file)
{
	FileViewWidget* widget = fileWidgets[file];
	fileWidgets.remove(file);
	ui.contentTabber->removeTab(ui.contentTabber->indexOf(widget));
	disconnect(file, NULL, this, NULL);
	delete widget;
}


void MainWindow::fileSaved(const File& file)
{
	DisplayedFile* dfile = (DisplayedFile*) sender();
	FileViewWidget* widget = fileWidgets[dfile];
	ui.contentTabber->setTabText(ui.contentTabber->indexOf(widget), file.getPath()->getFileName());
}


void MainWindow::fileChangeStatusChanged()
{
	DisplayedFile* dfile = (DisplayedFile*) sender();
	FileViewWidget* widget = fileWidgets[dfile];
	QString text = dfile->getFile().getPath()->getFileName();

	if (dfile->hasChanges())
		text += "*";

	ui.contentTabber->setTabText(ui.contentTabber->indexOf(widget), text);
}


void MainWindow::configurationChanged()
{
	//loadConfigUiSettings();
}


void MainWindow::taskLabelShouldAdjust()
{
}


void MainWindow::currentFileTabChanged(int index)
{
	QMap<DisplayedFile*, FileViewWidget*>::iterator it;
	QWidget* widget = ui.contentTabber->widget(index);

	for (it = fileWidgets.begin() ; it != fileWidgets.end() ; it++) {
		if (widget == it.value()) {
			System* sys = System::getInstance();
			DisplayedFile* file = it.key();
			sys->changeCurrentFile(file);
			break;
		}
	}
}


void MainWindow::fileTabClosed(int index)
{
	QMap<DisplayedFile*, FileViewWidget*>::iterator it;
	QWidget* widget = ui.contentTabber->widget(index);

	for (it = fileWidgets.begin() ; it != fileWidgets.end() ; it++) {
		if (widget == it.value()) {
			System* sys = System::getInstance();
			sys->closeFile(it.key());
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

