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

	findTextureInFileAction = new QAction(tr("Search Texture..."), mw);
	findTextureInFileAction->setShortcut(QKeySequence("Ctrl+T"));

	connect(findTextureAction, SIGNAL(triggered(bool)), this, SLOT(onFindTexture(bool)));
	connect(findTextureInFileAction, SIGNAL(triggered(bool)), this, SLOT(onFindTextureInFile(bool)));
}


TXDGUIModule::~TXDGUIModule()
{
	if (contextFile) {
		delete contextFile;
	}

	delete findTextureAction;
	delete findTextureInFileAction;
}


void TXDGUIModule::buildFileTreeMenu(const File& file, QMenu& menu)
{
	if (contextFile) {
		delete contextFile;
		contextFile = NULL;
	}

	if (file.isDirectory()  ||  file.isArchiveFile()) {
		menu.addAction(findTextureInFileAction);
		contextFile = new File(file);
	}
}


void TXDGUIModule::onFindTexture(bool checked)
{
	TextureSearchDialog dialog(mainWindow);
	dialog.exec();
}


void TXDGUIModule::onFindTextureInFile(bool checked)
{
	TextureSearchDialog dialog(mainWindow, contextFile);
	dialog.exec();
}

