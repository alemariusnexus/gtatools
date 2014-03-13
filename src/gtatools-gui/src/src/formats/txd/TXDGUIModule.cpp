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

#include "TXDGUIModule.h"
#include <QtGui/QAction>
#include "../../gui/MainWindow.h"
#include "TextureSearchDialog.h"
#include "TXDFormatHandler.h"
#include "../../ProfileManager.h"
#include "../../Profile.h"



TXDGUIModule::TXDGUIModule()
{
	findTextureAction = new QAction(tr("Search Texture..."), NULL);
	findTextureAction->setShortcut(QKeySequence("Ctrl+T"));

	findTextureInFileAction = new QAction(tr("Search Texture..."), NULL);
	findTextureInFileAction->setShortcut(QKeySequence("Ctrl+T"));

	ProfileManager* pm = ProfileManager::getInstance();

	connect(findTextureAction, SIGNAL(triggered(bool)), this, SLOT(onFindTexture(bool)));
	connect(findTextureInFileAction, SIGNAL(triggered(bool)), this, SLOT(onFindTextureInFile(bool)));
	connect(pm, SIGNAL(currentProfileChanged(Profile*, Profile*)), this,
			SLOT(currentProfileChanged(Profile*, Profile*)));
}


TXDGUIModule::~TXDGUIModule()
{
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


void TXDGUIModule::buildFileTreeMenu(const QLinkedList<File>& files, QMenu& menu)
{
	contextFiles = files;
	menu.addAction(findTextureInFileAction);
}


void TXDGUIModule::onFindTexture(bool checked)
{
	QLinkedList<File> files;

	Profile* profile = ProfileManager::getInstance()->getCurrentProfile();
	Profile::ResourceIterator it;

	for (it = profile->getResourceBegin() ; it != profile->getResourceEnd() ; it++) {
		File resfile = *it;
		files << resfile;
	}

	TXDFormatHandler::getInstance()->findTextureDialog(files, mainWindow);
}


void TXDGUIModule::onFindTextureInFile(bool checked)
{
	TXDFormatHandler::getInstance()->findTextureDialog(contextFiles, mainWindow);
}


void TXDGUIModule::currentProfileChanged(Profile* oldProfile, Profile* newProfile)
{
	findTextureAction->setEnabled(newProfile != NULL);
}
