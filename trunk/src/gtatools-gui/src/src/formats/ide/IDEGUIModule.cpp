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

#include "IDEGUIModule.h"
#include "../../gui/MainWindow.h"
#include "IDESearchWidget.h"
#include "../../ProfileManager.h"



IDEGUIModule::IDEGUIModule()
{
	findDefinitionAction = new QAction(tr("Search Item Definition"), NULL);
	findDefinitionAction->setShortcut(QKeySequence("Ctrl+D"));
	connect(findDefinitionAction, SIGNAL(triggered(bool)), this, SLOT(onFindDefinition(bool)));

	ProfileManager* pm = ProfileManager::getInstance();
	connect(pm, SIGNAL(currentProfileChanged(Profile*, Profile*)), this,
			SLOT(currentProfileChanged(Profile*, Profile*)));
}


IDEGUIModule::~IDEGUIModule()
{
	delete findDefinitionAction;
}


void IDEGUIModule::doInstall()
{
	QMenu* editMenu = mainWindow->getEditMenu();

	findDefinitionAction->setParent(mainWindow);
	editMenu->addAction(findDefinitionAction);
}


void IDEGUIModule::doUninstall()
{
	QMenu* editMenu = mainWindow->getEditMenu();

	findDefinitionAction->setParent(NULL);
	editMenu->removeAction(findDefinitionAction);
}


void IDEGUIModule::onFindDefinition(bool checked)
{
	IDESearchWidget dlg(mainWindow);
	dlg.exec();
}


void IDEGUIModule::currentProfileChanged(Profile* oldProfile, Profile* newProfile)
{
	findDefinitionAction->setEnabled(newProfile != NULL);
}
