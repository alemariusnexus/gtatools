/*
 * TXDGUIModule.cpp
 *
 *  Created on: 14.07.2010
 *      Author: alemariusnexus
 */

#include "TXDGUIModule.h"
#include <qaction.h>
#include "../../gui/MainWindow.h"
#include "TextureSearchDialog.h"



TXDGUIModule::TXDGUIModule(MainWindow* mw)
		: GUIModule(mw), contextFile(NULL)
{
	QMenu* editMenu = mw->getEditMenu();
	findTextureAction = new QAction(tr("Search Texture..."), mw);
	findTextureAction->setShortcut(QKeySequence("Ctrl+T"));
	editMenu->addAction(findTextureAction);
	connect(findTextureAction, SIGNAL(triggered(bool)), this, SLOT(onFindTexture(bool)));
}


TXDGUIModule::~TXDGUIModule()
{
	if (contextFile) {
		delete contextFile;
	}

	delete findTextureAction;
}


void TXDGUIModule::buildFileTreeMenu(const File& file, QMenu& menu)
{
	if (contextFile) {
		delete contextFile;
		contextFile = NULL;
	}

	if (file.isDirectory()  ||  file.isArchiveFile()) {
		menu.addAction(findTextureAction);
		contextFile = new File(file);
	}
}


void TXDGUIModule::onFindTexture(bool checked)
{
	TextureSearchDialog dialog(mainWindow, contextFile);
	dialog.exec();
}

