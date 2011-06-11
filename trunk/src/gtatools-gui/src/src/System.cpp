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

#include "System.h"
#include "formats/FormatManager.h"
#include <QtGui/QMessageBox>
#include <QtCore/QFile>
#include <QtCore/QDateTime>
#include <QtCore/QThread>
#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>
#include "DefaultDisplayedFile.h"




System::System()
		: mainWindow(NULL), currentFile(NULL)
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
	static System* inst = new System;
	return inst;
}


void System::initializeGL()
{
	sharedWidget = new QGLWidget;
}


bool System::openFile(const FileOpenRequest& request)
{
	if (isOpenFile(*request.getFile())) {
		return false;
	}

	FormatManager* fmgr = FormatManager::getInstance();
	FormatHandler* handler = fmgr->getHandler(*request.getFile());

	if (!handler) {
		return false;
	}

	DisplayedFile* dfile = handler->openFile(request);

	if (!dfile) {
		return false;
	}

	openFiles << dfile;

	try {
		emit fileOpened(request, dfile);
	} catch (...) {
		closeFile(*request.getFile());
		return false;
	}

	changeCurrentFile(dfile);
	return true;
}


void System::closeCurrentFile()
{
	if (currentFile != NULL) {
		closeFile(currentFile);
	}
}


bool System::hasOpenFile()
{
	return openFiles.size() != 0;
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


void System::changeCurrentFile(DisplayedFile* file)
{
	if (file) {
		DisplayedFile* prev = currentFile;
		currentFile = file;
		emit currentFileChanged(file, prev);
	} else {
		emit currentFileChanged(NULL, currentFile);
	}
}


DisplayedFile* System::findOpenFile(const File& file)
{
	QLinkedList<DisplayedFile*>::iterator it;

	for (it = openFiles.begin() ; it != openFiles.end() ; it++) {
		DisplayedFile* dfile = *it;

		if (dfile->getFile() == file) {
			return dfile;
		}
	}

	return NULL;
}


void System::closeFile(DisplayedFile* file)
{
	if (!file) {
		return;
	}

	openFiles.removeOne(file);

	if (currentFile == file) {
		if (openFiles.size() > 0) {
			changeCurrentFile(openFiles.first());
		} else {
			changeCurrentFile(NULL);
		}
	}

	emit fileClosed(file);

	delete file;
}


void System::logError(const QString& errmsg)
{
	emit errorLogged(errmsg);
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


SystemQueryResult System::sendSystemQuery(const SystemQuery& query)
{
	SystemQueryResult result(false);
	emit systemQuerySent(query, result);
	return result;
}



