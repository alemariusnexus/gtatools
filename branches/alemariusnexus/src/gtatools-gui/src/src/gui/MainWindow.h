/*
 * MainWindow.h
 *
 *  Created on: 11.07.2010
 *      Author: alemariusnexus
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

private:
	QModelIndex indexOfFileInTree(const File& file);
	void loadConfigUiSettings();

private slots:
	void taskStarted(int min, int max, const QString& message);
	void taskValueUpdated(int value);
	void taskEnded();
	void statusMessageShown(const QString& message, int timeout);
	void fileSelectedInTree(const QModelIndex& index);
	void currentProfileChanged(Profile* oldProfile, Profile* newProfile);
	void currentProfileContentChanged();
	void openFile(const File& file);
	void closeCurrentFile();
	void fileTreeContextMenuRequested(const QPoint& pos);
	void configurationChanged();

private:
	QTabWidget* contentTabCompact;
	Ui_MainWindow ui;
	QProgressBar* progressBar;
	QWidget* currentDisplayWidget;
	bool hasOpenFile;
};

#endif /* MAINWINDOW_H_ */
