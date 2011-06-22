/*
 * RWBSGUIModule.cpp
 *
 *  Created on: 21.06.2011
 *      Author: alemariusnexus
 */

#include "RWBSGUIModule.h"
#include "../../gui/HexEditorGotoDialog.h"
#include "../../gui/MainWindow.h"



RWBSGUIModule::RWBSGUIModule()
{
	gotoAction = new QAction(tr("Goto Offset..."), NULL);
	gotoAction->setShortcut(QKeySequence(tr("Ctrl+G")));
	connect(gotoAction, SIGNAL(triggered()), this, SLOT(gotoRequested()));
}


void RWBSGUIModule::doInstall()
{
	QMenu* editMenu = mainWindow->getEditMenu();

	editMenu->addAction(gotoAction);

	gotoAction->setParent(mainWindow);
}


void RWBSGUIModule::doUninstall()
{
	QMenu* editMenu = mainWindow->getEditMenu();

	editMenu->removeAction(gotoAction);

	gotoAction->setParent(NULL);
}


void RWBSGUIModule::gotoRequested()
{
	HexEditorGotoDialog dia(mainWindow);
	dia.exec(editor);
}
