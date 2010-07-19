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
