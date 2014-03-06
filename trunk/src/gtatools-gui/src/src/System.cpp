/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#include "System.h"
#include "formats/EntityManager.h"
#include <QtGui/QMessageBox>
#include <QtCore/QFile>
#include <QtCore/QDateTime>
#include <QtCore/QThread>
#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>
#include "DisplayedFile.h"
#include "ProfileManager.h"
#include <gta/scene/Scene.h>
#include <gta/scene/visibility/PVSDatabase.h>
#include <gta/resource/ResourceCache.h>



System* System::instance = NULL;



System::System()
		: mainWindow(NULL), currentEntity(NULL), initializing(true), shuttingDown(false)
{
	int dummyTexW = 16;
	int dummyTexH = 16;
	uint8_t* dummyTexData = new uint8_t[dummyTexW*dummyTexH*3];

	for (int i = 0 ; i < dummyTexH ; i++) {
		for (int j = 0 ; j < dummyTexW ; j++) {
			if ((i+j) % 2 == 0) {
				dummyTexData[(i*dummyTexW+j)*3] = 128;
				dummyTexData[(i*dummyTexW+j)*3 + 1] = 128;
				dummyTexData[(i*dummyTexW+j)*3 + 2] = 128;
			} else {
				dummyTexData[(i*dummyTexW+j)*3] = 255;
				dummyTexData[(i*dummyTexW+j)*3 + 1] = 255;
				dummyTexData[(i*dummyTexW+j)*3 + 2] = 255;
			}
		}
	}

	dummyTexImage = QImage(dummyTexData, dummyTexW, dummyTexH, QImage::Format_RGB888);
	dummyTexImage = dummyTexImage.scaled(128, 128);
}


System* System::getInstance()
{
	return instance;
}


void System::initialize()
{
	instance = new System;
	instance->initializeInstance();
}


void System::initializeInstance()
{
	initializeGL();

	Engine* engine = Engine::getInstance();

	engine->getMeshCache()->resize(10 * 1000000); // 10MB
	engine->getTextureCache()->resize(25 * 1000000); // 25MB
	engine->getAnimationCache()->resize(10 * 1000000); // 10MB

	ProfileManager::getInstance()->loadProfiles();

	MainWindow* win = new MainWindow;
	win->show();

	win->initialize();

	connect(qApp, SIGNAL(lastWindowClosed()), this, SLOT(destroyInstance()));

	EntityManager::getInstance();

	initializing = false;

	emit initializationDone();
}


void System::destroyInstance()
{
	emit aboutToQuit();

	shuttingDown = true;

	delete mainWindow;

	ProfileManager::destroy();
	Engine::destroy();
}


void System::initializeGL()
{
	sharedWidget = new QGLWidget;
}


void System::unhandeledException(Exception& ex)
{
	QDateTime dt = QDateTime::currentDateTime();

	QString logfileName(QString("exception-%1.log").arg(dt.toTime_t()));
	const char* bt = ex.getBacktrace();
	QFile logfile(logfileName);
	logfile.open(QFile::WriteOnly);

	printf("%s\n%s\n", ex.what(), ex.getBacktrace());

	logfile.write("UNHANDELED EXCEPTION REPORT ");
	logfile.write(dt.toString(Qt::ISODate).toAscii().constData());
	logfile.write(" gtatools version ");
	logfile.write(GTATOOLS_VERSION);
	logfile.write("\n\n");
	logfile.write("Unhandeled exception caught:\n\n");
	logfile.write(ex.what());
	logfile.write("\n\nBacktrace:\n\n");

	if (bt) {
		logfile.write(bt);
	} else {
		logfile.write("[Backtrace not supported on this system]");
	}

	logfile.flush();

	QString exText = tr("Unhandeled Exception %1%2").arg(ex.what()).arg(logfileName);

	if (QThread::currentThread() == qApp->thread()) {
		QMessageBox::critical(NULL, tr("Unhandeled Exception"), exText);
	} else {
		fprintf(stderr, "### Unhandeled exception caught ###\n");
		fprintf(stderr, "%s\n", exText.toLocal8Bit().constData());
	}
}


void System::emitConfigurationChange()
{
	emit configurationChanged();
}


void System::log(const LogEntry& entry)
{
	emit entryLogged(entry);
}


void System::showStatusMessage(const QString& message, int timeout)
{
	mainWindow->getStatusBar()->showMessage(message, timeout);
}


Task* System::createTask()
{
	Task* task = new Task(mainWindow);
	return task;
}


void System::installGUIModule(GUIModule* module)
{
	if (module->install(mainWindow)) {
		installedGUIModules << module;
		emit installedGUIModule(module);
	}
}


void System::uninstallGUIModule(GUIModule* module)
{
	if (isGUIModuleInstalled(module)) {
		if (module->uninstall()) {
			installedGUIModules.removeOne(module);
			emit uninstalledGUIModule(module);
		}
	}
}


void System::forceUninstallGUIModule(GUIModule* module)
{
	if (isGUIModuleInstalled(module)) {
		installedGUIModules.removeOne(module);
		module->forceUninstall();
		emit uninstalledGUIModule(module);
	}
}


QList<SystemQueryResult> System::sendSystemQuery(const SystemQuery& query)
{
	QList<SystemQueryResult> results;
	emit systemQuerySent(query, results);
	return results;
}


bool System::quit()
{
	while (!openEntities.isEmpty()) {
		DisplayedEntity* entity = openEntities.first();

		if (!closeEntity(entity, false)) {
			return false;
		}
	}

	// This will trigger signal qApp::lastWindowClosed(), which is connected to System::destroyInstance()
	mainWindow->close();

	return true;
}


DisplayedEntity* System::findSignatureOpenEntity(const QByteArray& sig)
{
	if (!sig.isEmpty()) {
		for (QLinkedList<DisplayedEntity*>::iterator it = openEntities.begin() ; it != openEntities.end() ; it++) {
			DisplayedEntity* oent = *it;
			QByteArray osig = oent->getSignature();

			if (!osig.isEmpty()  &&  sig.size() == osig.size()  &&  memcmp(sig.constData(), osig.constData(), sig.size()) == 0) {
				return oent;
			}
		}
	}

	return NULL;
}


bool System::openEntity(const EntityOpenRequest& req)
{
	EntityManager* man = EntityManager::getInstance();

	EntityHandler* handler = man->getEntityHandler(req);

	if (!handler)
		return false;

	QByteArray sig = handler->getEntitySignatureAheadOfTime(req);
	DisplayedEntity* oent = findSignatureOpenEntity(sig);

	if (oent) {
		changeCurrentEntity(oent);
		return false;
	}

	DisplayedEntity* entity = handler->openEntity(req);

	if (!entity)
		return false;

	sig = entity->getSignature();
	oent = findSignatureOpenEntity(sig);

	if (oent) {
		changeCurrentEntity(oent);
		delete entity;
		return false;
	}

	openEntities << entity;

	try {
		emit entityOpened(entity);
	} catch (...) {
		closeEntity(entity, true);
		return false;
	}

	changeCurrentEntity(entity);
	return true;
}


bool System::closeEntity(DisplayedEntity* entity, bool force)
{
	if (!entity)
		return false;

	if (!force  &&  entity->hasUnsavedChanges()) {
		QMessageBox::StandardButton res = QMessageBox::question(mainWindow, tr("Save changes?"),
				tr("'%1' has unsaved changes. Do you want to save before closing?").arg(entity->getName()),
				QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);

		if (res == QMessageBox::Yes) {
			entity->saveChanges(false);
		} else if (res == QMessageBox::No) {
		} else {
			return false;
		}
	}

	DisplayedEntity* newCurrentEntity = NULL;

	if (currentEntity == entity) {
		for (QLinkedList<DisplayedEntity*>::iterator it = openEntities.begin() ; it != openEntities.end() ; it++) {
			DisplayedEntity* oent = *it;

			if (oent != entity) {
				newCurrentEntity = oent;
				break;
			}
		}
	} else {
		newCurrentEntity = currentEntity;
	}

	if (newCurrentEntity != currentEntity) {
		changeCurrentEntity(newCurrentEntity);
	}

	openEntities.removeOne(entity);

	emit entityClosed(entity);

	delete entity;

	return true;
}


void System::changeCurrentEntity(DisplayedEntity* entity)
{
	if (currentEntity == entity)
		return;

	DisplayedEntity* oldEntity = currentEntity;

	emit currentEntityAboutToBeChanged(oldEntity, entity);

	currentEntity = entity;

	emit currentEntityChanged(oldEntity, entity);
}


bool System::openFile(const File& file)
{
	EntityOpenRequest req;

	req.setAttribute("type", "file");
	req.setAttribute("file", QString(file.getPath()->toString().get()));

	return openEntity(req);
}



