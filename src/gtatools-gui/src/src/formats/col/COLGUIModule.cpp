/*
 * COLGUIModule.cpp
 *
 *  Created on: 30.01.2011
 *      Author: alemariusnexus
 */

#include "COLGUIModule.h"
#include "../../gui/MainWindow.h"



COLGUIModule::COLGUIModule()
{
	wireframeAction = new QAction(tr("Show wireframe"), NULL);
	wireframeAction->setCheckable(true);
	wireframeAction->setChecked(false);

	connect(wireframeAction, SIGNAL(triggered(bool)), this, SLOT(wireframePropertyChangedSlot(bool)));
}


void COLGUIModule::wireframePropertyChangedSlot(bool wireframe)
{
	emit wireframePropertyChanged(wireframe);
}


void COLGUIModule::doInstall()
{
	QMenu* viewMenu = mainWindow->getViewMenu();
	viewMenu->addAction(wireframeAction);
}


void COLGUIModule::doUninstall()
{
	QMenu* viewMenu = mainWindow->getViewMenu();
	viewMenu->removeAction(wireframeAction);
}
