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

#include "GXTGUIModule.h"
#include <QtGui/QFileDialog>
#include <gtaformats/util/File.h>
#include <QtCore/QSettings>
#include "../../gui/MainWindow.h"
#include "GXTFormatHandler.h"
#include "GXTWidget.h"



GXTGUIModule::GXTGUIModule(GXTWidget* creator)
		: gxtWidget(creator)
{
	stringListMatchAction = new QAction(tr("String List Match"), NULL);
	connect(stringListMatchAction, SIGNAL(triggered(bool)), this, SLOT(onStringListMatch(bool)));

	iniExportAction = new QAction(tr("Export as INI file"), NULL);
	connect(iniExportAction, SIGNAL(triggered(bool)), this, SLOT(onIniExport(bool)));
}


GXTGUIModule::~GXTGUIModule()
{
	delete stringListMatchAction;
	delete iniExportAction;
}


void GXTGUIModule::doInstall()
{
	QMenu* fileMenu = mainWindow->getFileMenu();
	QMenu* editMenu = mainWindow->getEditMenu();

	stringListMatchAction->setParent(mainWindow);
	editMenu->addAction(stringListMatchAction);

	iniExportAction->setParent(mainWindow);
	fileMenu->addAction(iniExportAction);
}


void GXTGUIModule::doUninstall()
{
	QMenu* fileMenu = mainWindow->getFileMenu();
	QMenu* editMenu = mainWindow->getEditMenu();

	stringListMatchAction->setParent(NULL);
	editMenu->removeAction(stringListMatchAction);

	iniExportAction->setParent(NULL);
	fileMenu->removeAction(iniExportAction);
}


void GXTGUIModule::onStringListMatch(bool checked)
{
	QString fname = QFileDialog::getOpenFileName(mainWindow, tr("Choose a string list file"));

	if (!fname.isNull()) {
		File file(fname.toAscii().constData());
		QMap<QString, GXTTable*> tables = gxtWidget->getTables();
		GXTFormatHandler::getInstance()->stringListMatch(file, tables, mainWindow);
		gxtWidget->reloadCurrentTable();
	}
}


void GXTGUIModule::onIniExport(bool checked)
{
	File* gxtFile = gxtWidget->getOpenFile();
	QString fpath = QFileDialog::getSaveFileName(mainWindow, tr("Select a destination file"),
			QString("%1.ini").arg(gxtFile->getPath()->getFileName().get()), tr("INI Files (*.ini)"));

	if (!fpath.isNull()) {
		File file(fpath.toLocal8Bit().constData());
		QLinkedList<GXTTable*> tableList = gxtWidget->getSelectedTables();
		GXTFormatHandler::getInstance()->iniExport(file, tableList);
	}
}
