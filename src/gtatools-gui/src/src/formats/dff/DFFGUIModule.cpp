/*
 * DFFGUIModule.cpp
 *
 *  Created on: 27.07.2010
 *      Author: alemariusnexus
 */

#include "DFFGUIModule.h"
#include <qmenu.h>
#include "../../gui/MainWindow.h"
#include "DFFWidget.h"



DFFGUIModule::DFFGUIModule(DFFWidget* dffWidget)
{
	texturedAction = new QAction(tr("Show textured"), NULL);
	texturedAction->setCheckable(true);
	texturedAction->setChecked(true);

	wireframeAction = new QAction(tr("Show wireframe"), NULL);
	wireframeAction->setCheckable(true);
	wireframeAction->setChecked(false);

	dumpAction = new QAction(tr("Dump XML"), NULL);

	connect(texturedAction, SIGNAL(triggered(bool)), dffWidget, SLOT(texturedPropertyChanged(bool)));
	connect(wireframeAction, SIGNAL(triggered(bool)), dffWidget, SLOT(wireframePropertyChanged(bool)));
	connect(dumpAction, SIGNAL(triggered(bool)), dffWidget, SLOT(xmlDumpRequested(bool)));

}


void DFFGUIModule::doInstall()
{
	QMenu* viewMenu = mainWindow->getViewMenu();
	QMenu* fileMenu = mainWindow->getFileMenu();

	viewMenu->addAction(texturedAction);
	texturedAction->setParent(mainWindow);

	viewMenu->addAction(wireframeAction);
	wireframeAction->setParent(mainWindow);

	fileMenu->addAction(dumpAction);
	dumpAction->setParent(mainWindow);
}


void DFFGUIModule::doUninstall()
{
	QMenu* viewMenu = mainWindow->getViewMenu();

	viewMenu->removeAction(texturedAction);
	texturedAction->setParent(NULL);

	viewMenu->removeAction(wireframeAction);
	wireframeAction->setParent(NULL);
}

