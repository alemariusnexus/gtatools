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



System* System::getInstance()
{
	static System* inst = new System;
	return inst;
}


void System::openFile(const File& file, const QHash<QString, QVariant>& data)
{
	fileOpen = true;
	emit fileOpened(file, data);
}


void System::closeCurrentFile()
{
	emit currentFileClosed();
	fileOpen = false;
}


void System::startTask(int min, int max, const QString& message)
{
	emit taskStarted(min, max, message);
	updateTaskValue(min);
}


void System::updateTaskValue(int value)
{
	taskValue = value;
	emit taskValueUpdated(value);
}


void System::endTask()
{
	taskValue = 0;
	emit taskEnded();
}


void System::showStatusMessage(const QString& message, int timeout)
{
	emit statusMessageShown(message, timeout);
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

	QMessageBox::critical(NULL, tr("Unhandeled Exception"), tr("Unhandeled Exception %1%2").arg(ex.what()).arg(logfileName));
}


void System::emitConfigurationChange()
{
	emit configurationChanged();
}


void System::installGUIModule(GUIModule* module)
{
	installedGUIModules << module;
	module->install(mainWindow);
	emit installedGUIModule(module);
}


void System::uninstallGUIModule(GUIModule* module)
{
	installedGUIModules.removeOne(module);
	module->uninstall();
	emit uninstalledGUIModule(module);
}

