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

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <nxcommon/file/File.h>
#include "ui_MainFrame.h"
#include <QMainWindow>
#include "../Profile.h"
#include "GUIModule.h"
#include <QAbstractItemModel>
#include <QMenu>
#include <QPoint>
#include <QProgressBar>
#include <QTabWidget>
#include <QLabel>
#include <QMap>
#include "../EntityOpenRequest.h"
#include "FileViewWidget.h"
#include "FileTree.h"
#include <QAction>
#include "../DisplayedFile.h"
#include <QCloseEvent>


class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();
	void initialize();
	QMenuBar* getMenuBar() { return ui.menuBar; }
	QMenu* getSettingsMenu() { return ui.menuSettings; }
	QMenu* getProfileMenu() { return ui.menuProfile; }
	QMenu* getEditMenu() { return ui.menuEdit; }
	QMenu* getHelpMenu() { return ui.menuHelp; }
	QMenu* getFileMenu() { return ui.menuFile; }
	QMenu* getViewMenu() { return ui.menuView; }
	QMenu* getToolsMenu() { return ui.menuTools; }
	QToolBar* getToolBar() { return ui.toolBar; }
	QProgressBar* getProgressBar() { return progressBar; }
	QStatusBar* getStatusBar() { return ui.statusbar; }
	QLabel* getTaskLabel() { return taskLabel; }
	void addDockWidget(Qt::DockWidgetArea area, QDockWidget* widget);
	void removeDockWidget(QDockWidget* widget);

protected:
	virtual void closeEvent(QCloseEvent* evt);

private slots:
	void configurationChanged();
	void dockWidgetViewChanged(bool checked);
	void dockWidgetVisibilityChanged(bool visible);
	void openEntity(DisplayedEntity* entity);
	void closeEntity(DisplayedEntity* entity);
	void currentEntityChanged(DisplayedEntity* oe, DisplayedEntity* ne);
	void centralTabWidgetCloseRequested(int index);
	void centralTabWidgetCurrentChanged(int index);
	void loadWindowState();
	void entityChangeStatusChanged(bool hasChanges);

public slots:
	void taskLabelShouldAdjust();

private:
	QList<QDockWidget*> dockWidgets;
	QList<QAction*> dockViewActions;
	Ui_MainWindow ui;
	QLabel* taskLabel;
	QProgressBar* progressBar;
	QTabWidget* centralTabber;
	QMap<DisplayedEntity*, QWidget*> entityWidgets;
};

#endif /* MAINWINDOW_H_ */
