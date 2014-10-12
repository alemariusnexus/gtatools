/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

	EntityOpenRequest req;

	req.setAttribute("type", "IFP::AnimViewer");

	sys->openEntity(req);

	/*IFPAnimationViewerWidget* viewer = new IFPAnimationViewerWidget;
	IFPAnimationViewerEntity* ent = new IFPAnimationViewerEntity(viewer);

	sys->openEntity(ent);*/
}
