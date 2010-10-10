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

#include "GXTGUIModule.h"
#include <QtGui/QFileDialog>
#include <gtaformats/util/File.h>
#include <QtCore/QSettings>
#include "../../gui/MainWindow.h"
#include "GXTWidget.h"



GXTGUIModule::GXTGUIModule(GXTWidget* creator)
		: gxtWidget(creator)
{
	stringListMatchAction = new QAction(tr("String List Match"), NULL);
	connect(stringListMatchAction, SIGNAL(triggered(bool)), this, SLOT(onStringListMatch(bool)));
}


void GXTGUIModule::doInstall()
{
	QMenu* editMenu = mainWindow->getEditMenu();

	stringListMatchAction->setParent(mainWindow);
	editMenu->addAction(stringListMatchAction);
}


void GXTGUIModule::doUninstall()
{
	QMenu* editMenu = mainWindow->getEditMenu();

	stringListMatchAction->setParent(NULL);
	editMenu->removeAction(stringListMatchAction);
}


void GXTGUIModule::onStringListMatch(bool checked)
{
	QString fname = QFileDialog::getOpenFileName(mainWindow, tr("Choose a string list file"));

	if (!fname.isNull()) {
		File file(fname.toAscii().constData());
		gxtWidget->stringListMatch(file);
	}
}
