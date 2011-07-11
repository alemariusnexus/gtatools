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
#include <QtOpenGL/QGLWidget>
#include "Task.h"
#include "FileOpenRequest.h"
#include "SystemQuery.h"
#include "SystemQueryResult.h"
#include "DisplayedFile.h"
#include <gtaformats/util/FileFinder.h>
#include <QtGui/QImage>
#include "LogEntry.h"


class System : public QObject {
	Q_OBJECT

public:
	static System* getInstance();
	static void initialize();

public:
	void initializeGL();
	bool openFile(const FileOpenRequest& request);
	bool closeFile(const File& file, bool force = false) { return closeFile(findOpenFile(file), force); }
	bool closeFile(DisplayedFile* file, bool force = false);
	bool closeCurrentFile(bool force = false);
	void changeCurrentFile(DisplayedFile* file);
	void changeCurrentFile(const File& file) { changeCurrentFile(findOpenFile(file)); }
	DisplayedFile* findOpenFile(const File& file);
	DisplayedFile* getCurrentFile() { return currentFile; }
	QLinkedList<DisplayedFile*> getOpenFiles() { return openFiles; }
	bool isOpenFile(const File& file) { return findOpenFile(file) != NULL; }
	bool hasOpenFile();
	void unhandeledException(Exception& ex);
	void emitConfigurationChange();
	QLinkedList<GUIModule*> getInstalledGUIModules() { return installedGUIModules; }
	bool isGUIModuleInstalled(GUIModule* module) { return installedGUIModules.contains(module); }
	void log(const LogEntry& entry);
	void showStatusMessage(const QString& message, int timeout = 4000);
	Task* createTask();
	void installGUIModule(GUIModule* module);
	void uninstallGUIModule(GUIModule* module);
	void forceUninstallGUIModule(GUIModule* module);
	SystemQueryResult sendSystemQuery(const SystemQuery& query);
	QGLWidget* getSharedGLWidget() { return sharedWidget; }
	QImage getDummyTextureImage() { return dummyTexImage; }
	bool quit();
	bool isInitializing() const { return initializing; }
	bool isShuttingDown() const { return shuttingDown; }

signals:
	void fileOpened(const FileOpenRequest& request, DisplayedFile* file);
	void fileClosed(DisplayedFile* file);
	void currentFileChanged(DisplayedFile* current, DisplayedFile* prev);
	void currentFileClosed();
	void configurationChanged();
	void installedGUIModule(GUIModule* module);
	void uninstalledGUIModule(GUIModule* module);
	void entryLogged(const LogEntry& entry);
	void systemQuerySent(const SystemQuery& query, SystemQueryResult& result);
	void aboutToQuit();
	void initializationDone();

private:
	System();
	void setMainWindow(MainWindow* mw) { mainWindow = mw; }
	void initializeInstance();

private slots:
	void destroyInstance();

private:
	static System* instance;

private:
	MainWindow* mainWindow;
	QLinkedList<GUIModule*> installedGUIModules;
	QLinkedList<DisplayedFile*> openFiles;
	DisplayedFile* currentFile;
	QGLWidget* sharedWidget;
	QImage dummyTexImage;
	bool initializing, shuttingDown;

private:
	friend class MainWindow;
};

#endif /* SYSTEM_H_ */
