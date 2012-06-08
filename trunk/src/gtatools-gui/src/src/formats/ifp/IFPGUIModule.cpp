/*
 * IFPGUIModule.cpp
 *
 *  Created on: 27.05.2012
 *      Author: alemariusnexus
 */

#include "IFPGUIModule.h"
#include "../../gui/MainWindow.h"
#include "IFPAnimationViewerWidget.h"
#include "IFPAnimationViewerEntity.h"
#include "../../System.h"



IFPGUIModule::IFPGUIModule()
{
	animViewerAction = new QAction(tr("Animation Viewer"), NULL);
	connect(animViewerAction, SIGNAL(triggered(bool)), this, SLOT(animViewerRequested(bool)));
}


IFPGUIModule::~IFPGUIModule()
{
	delete animViewerAction;
}


void IFPGUIModule::doInstall()
{
	QMenu* toolsMenu = mainWindow->getToolsMenu();

	toolsMenu->addAction(animViewerAction);
	animViewerAction->setParent(mainWindow);
}


void IFPGUIModule::doUninstall()
{
	QMenu* toolsMenu = mainWindow->getToolsMenu();

	animViewerAction->setParent(NULL);
	toolsMenu->removeAction(animViewerAction);
}


void IFPGUIModule::animViewerRequested(bool)
{
	System* sys = System::getInstance();

	IFPAnimationViewerWidget* viewer = new IFPAnimationViewerWidget;
	IFPAnimationViewerEntity* ent = new IFPAnimationViewerEntity(viewer);

	sys->openEntity(ent);
}
