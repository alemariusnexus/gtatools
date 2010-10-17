/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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

#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <gtaformats/util/File.h>
#include "ui_MainFrame.h"
#include <qmainwindow.h>
#include "../Profile.h"
#include "GUIModule.h"
#include <qabstractitemmodel.h>
#include <qmenu.h>
#include <qpoint.h>
#include <qprogressbar.h>
#include <qtabwidget.h>
#include <QLabel>
#include <QtCore/qmap.h>
#include "../FileOpenRequest.h"
#include "FileViewWidget.h"
#include "FileTree.h"
#include <QtGui/qaction.h>


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
	QProgressBar* getProgressBar() { return progressBar; }
	QStatusBar* getStatusBar() { return ui.statusbar; }
	QLabel* getTaskLabel() { return taskLabel; }
	void addDockWidget(Qt::DockWidgetArea area, QDockWidget* widget);
	void removeDockWidget(QDockWidget* widget);

private slots:
	void openFile(const FileOpenRequest& request);
	void currentFileChanged(File* file, File* prev);
	void closeFile(File* file);
	void configurationChanged();
	void currentFileTabChanged(int index);
	void fileTabClosed(int index);
	void dockWidgetViewChanged(bool checked);
	void dockWidgetVisibilityChanged(bool visible);

public slots:
	void taskLabelShouldAdjust();

private:
	QList<QDockWidget*> dockWidgets;
	QList<QAction*> dockViewActions;
	QMap<File, FileViewWidget*> fileWidgets;
	Ui_MainWindow ui;
	QLabel* taskLabel;
	QProgressBar* progressBar;
};

#endif /* MAINWINDOW_H_ */
