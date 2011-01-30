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

#include <QtCore/QObject>
#include <gtaformats/util/File.h>
#include <gtaformats/util/Exception.h>
#include "gui/GUIModule.h"
#include <QtCore/QLinkedList>
#include "gui/MainWindow.h"
#include <QtCore/QHash>
#include <QtCore/QVariant>
#include <QtCore/QString>
#include "Task.h"
#include "FileOpenRequest.h"
#include "SystemQuery.h"
#include "SystemQueryResult.h"
#include <gtaformats/util/FileFinder.h>


class System : public QObject {
	Q_OBJECT

public:
	static System* getInstance();

public:
	bool openFile(const FileOpenRequest& request);
	void closeFile(const File& file);
	void closeCurrentFile();
	void changeCurrentFile(const File* file);
	File* getCurrentFile() { return currentFile; }
	QLinkedList<File*> getOpenFiles() { return openFiles; }
	bool isOpenFile(const File& file);
	bool hasOpenFile();
	void unhandeledException(Exception& ex);
	void emitConfigurationChange();
	QLinkedList<GUIModule*> getInstalledGUIModules() { return installedGUIModules; }
	bool isGUIModuleInstalled(GUIModule* module) { return installedGUIModules.contains(module); }
	void logError(const QString& errmsg);
	void showStatusMessage(const QString& message, int timeout = 4000);
	Task* createTask();
	void installGUIModule(GUIModule* module);
	void uninstallGUIModule(GUIModule* module);
	SystemQueryResult sendSystemQuery(const SystemQuery& query);

signals:
	void fileOpened(const FileOpenRequest& request);
	void fileClosed(File* file);
	void currentFileChanged(File* current, File* prev);
	void currentFileClosed();
	void configurationChanged();
	void installedGUIModule(GUIModule* module);
	void uninstalledGUIModule(GUIModule* module);
	void errorLogged(const QString& errmsg);
	void systemQuerySent(const SystemQuery& query, SystemQueryResult& result);

private:
	System() : mainWindow(NULL), currentFile(NULL) {}
	void setMainWindow(MainWindow* mw) { mainWindow = mw; }

private:
	MainWindow* mainWindow;
	QLinkedList<GUIModule*> installedGUIModules;
	QLinkedList<File*> openFiles;
	File* currentFile;

private:
	friend class MainWindow;
};

#endif /* SYSTEM_H_ */
