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

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <gta/gl.h>
#include <QtCore/QObject>
#include <nxcommon/file/File.h>
#include <nxcommon/exception/Exception.h>
#include "gui/GUIModule.h"
#include <QtCore/QLinkedList>
#include "gui/MainWindow.h"
#include <QtCore/QHash>
#include <QtCore/QVariant>
#include <QtCore/QString>
#include <QtOpenGL/QGLWidget>
#include "Task.h"
#include "EntityOpenRequest.h"
#include "SystemQuery.h"
#include "SystemQueryResult.h"
#include "DisplayedEntity.h"
#include "DisplayedFile.h"
#include <nxcommon/file/FileFinder.h>
#include <QImage>
#include "LogEntry.h"


class System : public QObject {
	Q_OBJECT

public:
	static System* getInstance();
	static void initialize();

public:
	void initializeGL();
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
	QList<SystemQueryResult> sendSystemQuery(const SystemQuery& query);
	QGLWidget* getSharedGLWidget() { return sharedWidget; }
	QImage getDummyTextureImage() { return dummyTexImage; }
	bool quit();
	bool isInitializing() const { return initializing; }
	bool isShuttingDown() const { return shuttingDown; }
	MainWindow* getMainWindow() { return mainWindow; }

	bool openEntity(const EntityOpenRequest& req);
	bool closeEntity(DisplayedEntity* entity, bool force = false);
	void changeCurrentEntity(DisplayedEntity* entity);
	DisplayedEntity* getCurrentEntity() { return currentEntity; }
	bool hasOpenEntity() const { return !openEntities.empty(); }

	bool openFile(const File& file);

signals:
	void configurationChanged();
	void installedGUIModule(GUIModule* module);
	void uninstalledGUIModule(GUIModule* module);
	void entryLogged(const LogEntry& entry);
	void systemQuerySent(const SystemQuery& query, QList<SystemQueryResult>& results);
	void aboutToQuit();
	void initializationDone();

	void entityOpened(DisplayedEntity* entity);
	void entityClosed(DisplayedEntity* entity);
	void currentEntityAboutToBeChanged(DisplayedEntity* oldEnt, DisplayedEntity* newEnt);
	void currentEntityChanged(DisplayedEntity* oldEnt, DisplayedEntity* newEnt);

private:
	System();
	void setMainWindow(MainWindow* mw) { mainWindow = mw; }
	void initializeInstance();
	DisplayedEntity* findSignatureOpenEntity(const QByteArray& sig);

private slots:
	void destroyInstance();

private:
	static System* instance;

private:
	MainWindow* mainWindow;
	QLinkedList<GUIModule*> installedGUIModules;
	QLinkedList<DisplayedEntity*> openEntities;
	DisplayedEntity* currentEntity;
	QGLWidget* sharedWidget;
	QImage dummyTexImage;
	bool initializing, shuttingDown;

private:
	friend class MainWindow;
};

#endif /* SYSTEM_H_ */
