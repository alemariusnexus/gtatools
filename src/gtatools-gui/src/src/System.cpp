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

#include "System.h"
#include "formats/EntityManager.h"
#include <gta/gl.h>
#include <QtCore/QFile>
#include <QtCore/QDateTime>
#include <QtCore/QThread>
#include <QInputDialog>
#include <QMessageBox>
#include <QtOpenGL/QGLFormat>
#include "DisplayedFile.h"
#include "ProfileManager.h"
#include <gta/scene/Scene.h>
#include <gta/scene/visibility/PVSDatabase.h>
#include <nxcommon/ResourceCache.h>



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
	// We will destroy and rebuild it with the chosen OpenGL format in the end, but we need to have a valid current OpenGL
	// context for calling some of the methods below.
	//sharedWidget = new QGLWidget;
	//sharedWidget->makeCurrent();

	struct _GLVersion
	{
		unsigned int majorVer;
		unsigned int minorVer;
	};

	// Array of all the OpenGL versions that gtatools-gui might work with. We will try to build a context with the first entry
	// in this array, and if that fails, we go on trying the next one and so on.
	_GLVersion glVersionsToTry[] = {
			{ 4, 3 },
			{ 4, 2 },
			{ 4, 1 },
			{ 4, 0 },
			{ 3, 3 },
			{ 3, 2 },
			{ 3, 1 },
			{ 3, 0 }
	};

	QGLFormat::OpenGLVersionFlags vflags = QGLFormat::openGLVersionFlags();

	QGLFormat defaultGlFormat;
	defaultGlFormat.setDoubleBuffer(true);
	defaultGlFormat.setDirectRendering(true);
	defaultGlFormat.setDepth(true);
	defaultGlFormat.setAlpha(true);

	// NOTE: This line is VERY important. GLEW (tested with version 1.11) does not work with OpenGL 3.2+ Core Contexts,
	// apparently because it calls glGetString(GL_EXTENSIONS), which is deprecated and therefore returns an error on the
	// Core profile, which makes GLEW fail at basically everything.
	// This issue is described in http://www.opengl.org/wiki/OpenGL_Loading_Library. Quote:
	//
	// 		"GLEW has a problem with core contexts. It calls glGetString(GL_EXTENSIONS), which causes GL_INVALID_ENUM on GL
	//		 3.2+ core context as soon as glewInit() is called. It also doesn't fetch the function pointers. The solution
	//		 is for GLEW to use glGetStringi instead. The current version of GLEW is 1.10.0 but they still haven't corrected
	// 		 it. The only fix is to use glewExperimental for now"
	//
	// Instead of using glewExperimental, we simply use the compatibility profile (which is probably a good idea in general,
	// god knows how many "deprecated" (-> compatibility!) features I use in libgta that might fail with a core context...
	defaultGlFormat.setProfile(QGLFormat::CompatibilityProfile);

	int majorVer = 0;
	int minorVer = 0;
	bool foundMatching = false;

	for (size_t i = 0 ; i < sizeof(glVersionsToTry)/sizeof(_GLVersion) ; i++) {
		_GLVersion ver = glVersionsToTry[i];

		defaultGlFormat.setVersion(ver.majorVer, ver.minorVer);

		sharedWidget = new QGLWidget(defaultGlFormat);
		sharedWidget->makeCurrent();

		majorVer = sharedWidget->format().majorVersion();
		minorVer = sharedWidget->format().minorVersion();

		if (majorVer > ver.majorVer  ||  (majorVer == ver.majorVer  &&  minorVer >= ver.minorVer)) {
			foundMatching = true;
			break;
		} else {
			delete sharedWidget;
		}
	}

	if (!foundMatching) {
		fprintf(stderr, "ERROR: OpenGL >= 3.0 seems to be unsupported on the system. gtatools-gui will need at least OpenGL "
				"3.0 to work correctly!\n");
	}

	printf("Using OpenGL version %d.%d\n", majorVer, minorVer);

	QGLFormat::setDefaultFormat(defaultGlFormat);
}


void System::unhandeledException(Exception& ex)
{
	QDateTime dt = QDateTime::currentDateTime();

	QString logfileName(QString("exception-%1.log").arg(dt.toTime_t()));
	CString bt = ex.getBacktrace();
	QFile logfile(logfileName);
	logfile.open(QFile::WriteOnly);

	printf("%s\n%s\n", ex.what(), bt.get());

	logfile.write("UNHANDELED EXCEPTION REPORT ");
	logfile.write(dt.toString(Qt::ISODate).toLocal8Bit().constData());
	logfile.write(" gtatools version ");
	logfile.write(GTATOOLS_VERSION);
	logfile.write("\n\n");
	logfile.write("Unhandeled exception caught:\n\n");
	logfile.write(ex.what());
	logfile.write("\n\nBacktrace:\n\n");

	if (!bt.isNull()) {
		logfile.write(bt.get());
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
	req.setAttribute("file", QString(file.getPath().toString().get()));

	return openEntity(req);
}



