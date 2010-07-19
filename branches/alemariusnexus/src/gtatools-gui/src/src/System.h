/*
 * System.h
 *
 *  Created on: 15.07.2010
 *      Author: alemariusnexus
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <qobject.h>
#include <gtaformats/util/File.h>
#include <gtaformats/util/Exception.h>
#include "gui/GUIModule.h"
#include <qlinkedlist.h>
#include "gui/MainWindow.h"


class System : public QObject {
	Q_OBJECT

public:
	static System* getInstance();

public:
	void openFile(const File& file);
	void closeCurrentFile();
	bool hasOpenFile() { return fileOpen; }
	void startTask(int min, int max, const QString& message);
	void updateTaskValue(int value);
	int getTaskValue() { return taskValue; }
	void endTask();
	void showStatusMessage(const QString& message, int timeout = 4000);
	void unhandeledException(Exception& ex);
	void emitConfigurationChange();
	void installGUIModule(GUIModule* module);
	void uninstallGUIModule(GUIModule* module);
	QLinkedList<GUIModule*> getInstalledGUIModules() { return installedGUIModules; }

signals:
	void fileOpened(const File& file);
	void currentFileClosed();
	void taskStarted(int min, int max, const QString& message);
	void taskValueUpdated(int value);
	void taskEnded();
	void statusMessageShown(const QString& message, int timeout);
	void configurationChanged();
	void installedGUIModule(GUIModule* module);
	void uninstalledGUIModule(GUIModule* module);

private:
	System() {}
	void setMainWindow(MainWindow* mw) { mainWindow = mw; }

private:
	MainWindow* mainWindow;
	QLinkedList<GUIModule*> installedGUIModules;
	bool fileOpen;
	int taskValue;

private:
	friend class MainWindow;
};

#endif /* SYSTEM_H_ */
