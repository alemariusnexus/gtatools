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

#include "RWBSGUIModule.h"
#include "../../gui/HexEditorGotoDialog.h"
#include "../../gui/MainWindow.h"



RWBSGUIModule::RWBSGUIModule()
{
	gotoAction = new QAction(tr("Goto Offset..."), NULL);
	gotoAction->setShortcut(QKeySequence(tr("Ctrl+G")));
	connect(gotoAction, SIGNAL(triggered()), this, SLOT(gotoRequested()));

	utilDockWidget = new QDockWidget(tr("RenderWare Binary Stream File View"));
	utilDockWidget->setObjectName("rwbsUtilDock");

	analyzerDockWidget = new QDockWidget(tr("RenderWare Binary Stream Analyzer View"));
	analyzerDockWidget->setObjectName("rwbsAnalyzerDock");

	utilWidget = new RWBSUtilityWidget(utilDockWidget);
	utilDockWidget->setWidget(utilWidget);

	/*analyzerWidget = new RWBSAnalyzerWidget(analyzerDockWidget);
	analyzerDockWidget->setWidget(analyzerWidget);*/
}


void RWBSGUIModule::doInstall()
{
	mainWindow->addDockWidget(Qt::BottomDockWidgetArea, utilDockWidget);
	utilDockWidget->setParent(mainWindow);

	/*mainWindow->addDockWidget(Qt::BottomDockWidgetArea, analyzerDockWidget);
	analyzerDockWidget->setParent(mainWindow);*/

	QMenu* editMenu = mainWindow->getEditMenu();

	editMenu->addAction(gotoAction);

	gotoAction->setParent(mainWindow);
}


void RWBSGUIModule::doUninstall()
{
	mainWindow->removeDockWidget(utilDockWidget);
	utilDockWidget->setParent(NULL);

	/*mainWindow->removeDockWidget(analyzerDockWidget);
	analyzerDockWidget->setParent(NULL);*/

	QMenu* editMenu = mainWindow->getEditMenu();

	editMenu->removeAction(gotoAction);

	gotoAction->setParent(NULL);
}


void RWBSGUIModule::gotoRequested()
{
	HexEditorGotoDialog dia(mainWindow);
	dia.exec(editor);
}
