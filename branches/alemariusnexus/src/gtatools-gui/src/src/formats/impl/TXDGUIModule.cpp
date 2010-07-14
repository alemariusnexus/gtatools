/*
 * TXDGUIModule.cpp
 *
 *  Created on: 14.07.2010
 *      Author: alemariusnexus
 */

#include "TXDGUIModule.h"
#include <qaction.h>
#include "../../gui/MainWindow.h"



void TXDGUIModule::apply(MainWindow* mw)
{
	QMenu* editMenu = mw->getEditMenu();
	findTextureAction = new QAction("Search Texture...", mw);
	editMenu->addAction(findTextureAction);
	connect(findTextureAction, SIGNAL(triggered(bool)), this, SLOT(onFindTexture(bool)));
}


void TXDGUIModule::remove(MainWindow* mw)
{
	delete findTextureAction;
}


void TXDGUIModule::onFindTexture(bool checked)
{
	printf("Fundelt!\n");
}

