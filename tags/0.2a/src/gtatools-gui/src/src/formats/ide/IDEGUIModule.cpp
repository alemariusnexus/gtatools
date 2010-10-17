/*
 * IDEGUIModule.cpp
 *
 *  Created on: 15.10.2010
 *      Author: alemariusnexus
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
