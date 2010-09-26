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

#include "System.h"
#include <qmessagebox.h>
#include <qfile.h>
#include <qdatetime.h>
#include <QThread>



System* System::getInstance()
{
	static System* inst = new System;
	return inst;
}


bool System::openFile(const FileOpenRequest& request)
{
	if (isOpenFile(*request.getFile())) {
		return false;
	}

	openFiles << new File(*request.getFile());
	emit fileOpened(request);

	changeCurrentFile(request.getFile());
}


bool System::isOpenFile(const File& file)
{
	QLinkedList<File*>::iterator it;

	for (it = openFiles.begin() ; it != openFiles.end() ; it++) {
		File* ofile = *it;

		if (*ofile == file) {
			return true;
		}
	}

	return false;
}


void System::closeCurrentFile()
{
	if (currentFile != NULL) {
		closeFile(*currentFile);
	}
}


bool System::hasOpenFile()
{
	return openFiles.size() != 0;
}


Task* System::createTask()
{
	Task* task = new Task(mainWindow);
	return task;
}


void System::showStatusMessage(const QString& message, int timeout)
{
	mainWindow->getStatusBar()->showMessage(message, timeout);
}


void System::unhandeledException(Exception& ex)
{
	QDateTime dt = QDateTime::currentDateTime();

	QString logfileName(QString("exception-%1.log").arg(dt.toTime_t()));
	char* bt = ex.getBacktrace();
	QFile logfile(logfileName);
	logfile.open(QFile::WriteOnly);

	logfile.write("UNHANDELED EXCEPTION REPORT ");
	logfile.write(dt.toString(Qt::ISODate).toAscii().constData());
	logfile.write("\n\n");
	logfile.write("Unhandeled exception caught:\n\n");
	logfile.write(ex.what());
	logfile.write("\n\nBacktrace:\n\n");
	logfile.write(bt);
	logfile.flush();

	delete[] bt;

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


void System::changeCurrentFile(const File* file)
{
	QLinkedList<File*>::iterator it;

	for (it = openFiles.begin() ; it != openFiles.end() ; it++) {
		File* ofile = *it;

		if (*ofile == *file) {
			File* prev = currentFile;
			currentFile = ofile;
			emit currentFileChanged(ofile, prev);
			break;
		}
	}
}


void System::closeFile(const File& file)
{
	QLinkedList<File*>::iterator it;

	for (it = openFiles.begin() ; it != openFiles.end() ; it++) {
		File* ofile = *it;

		if (*ofile == file) {
			openFiles.removeOne(ofile);

			if (currentFile == ofile) {
				if (openFiles.size() > 0) {
					changeCurrentFile(openFiles.first());
				} else {
					changeCurrentFile(NULL);
				}
			}

			emit fileClosed(ofile);
			break;
		}
	}
}


void System::installGUIModule(GUIModule* module)
{
	if (!isGUIModuleInstalled(module)) {
		installedGUIModules << module;
		module->install(mainWindow);
		emit installedGUIModule(module);
	}
}


void System::uninstallGUIModule(GUIModule* module)
{
	if (isGUIModuleInstalled(module)) {
		installedGUIModules.removeOne(module);
		module->uninstall();
		emit uninstalledGUIModule(module);
	}
}

