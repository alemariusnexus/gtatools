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


class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow();
	~MainWindow();
	void initialize();
	void openFile(const File& file);
	QMenuBar* getMenuBar() { return ui.menuBar; }
	QMenu* getSettingsMenu() { return ui.menuSettings; }
	QMenu* getProfileMenu() { return ui.menuProfile; }
	QMenu* getEditMenu() { return ui.menuEdit; }

private slots:
	void fileSelectedInTree(const QModelIndex& index);
	void currentProfileChanged(Profile* oldProfile, Profile* newProfile);
	void currentProfileContentChanged();
	void settingsRequested(bool checked);
	void profileSwitchRequested(QAction* action);

private:
	Ui_MainWindow ui;
	QLinkedList<GUIModule*> globalModules;
	QLinkedList<GUIModule*> currentFileModules;
};

#endif /* MAINWINDOW_H_ */
