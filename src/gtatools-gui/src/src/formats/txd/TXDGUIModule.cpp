/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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
 */

#include "TXDGUIModule.h"
#include <qaction.h>
#include "../../gui/MainWindow.h"
#include "TextureSearchDialog.h"



TXDGUIModule::TXDGUIModule()
		: contextFile(NULL)
{
	findTextureAction = new QAction(tr("Search Texture..."), NULL);
	findTextureAction->setShortcut(QKeySequence("Ctrl+T"));

	findTextureInFileAction = new QAction(tr("Search Texture..."), NULL);
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


void TXDGUIModule::doInstall()
{
	findTextureAction->setParent(mainWindow);
	findTextureInFileAction->setParent(mainWindow);

	QMenu* editMenu = mainWindow->getEditMenu();
	editMenu->addAction(findTextureAction);
}


void TXDGUIModule::doUninstall()
{
	findTextureAction->setParent(NULL);
	findTextureInFileAction->setParent(NULL);

	QMenu* editMenu = mainWindow->getEditMenu();
	editMenu->removeAction(findTextureAction);
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

