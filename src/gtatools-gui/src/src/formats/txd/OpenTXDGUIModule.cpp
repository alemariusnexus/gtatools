/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#include "OpenTXDGUIModule.h"
#include "TXDWidget.h"
#include <cstdio>
#include "../../gui/MainWindow.h"
#include <gtaformats/txd/TXDArchive.h>
#include <gtaformats/txd/TXDTextureHeader.h>
#include <gtaformats/config.h>
#include <QtCore/QLinkedList>
#include <QtGui/QFileDialog>
#include <QtGui/QImageWriter>
#include "TXDFormatHandler.h"
#include "../../System.h"




OpenTXDGUIModule::OpenTXDGUIModule(TXDWidget* txdWidget)
		: txdWidget(txdWidget)
{
	extractAction = new QAction(tr("Extract textures..."), NULL);
	extractAction->setShortcut(Qt::CTRL + Qt::Key_E);
	extractAction->setEnabled(txdWidget->getArchive() != NULL);
	connect(extractAction, SIGNAL(triggered(bool)), this, SLOT(onExtract(bool)));
}


OpenTXDGUIModule::~OpenTXDGUIModule()
{
	delete extractAction;
}


void OpenTXDGUIModule::doInstall()
{
	QMenu* fileMenu = mainWindow->getFileMenu();
	fileMenu->addAction(extractAction);
}


void OpenTXDGUIModule::doUninstall()
{
	QMenu* fileMenu = mainWindow->getFileMenu();
	fileMenu->removeAction(extractAction);
}


void OpenTXDGUIModule::onExtract(bool checked)
{
	TXDArchive* txd = txdWidget->getArchive();
	QLinkedList<TXDTextureHeader*> textures = txdWidget->getSelectedTextures();
	TXDFormatHandler::getInstance()->extractTexturesDialog(txd, textures, mainWindow);
}

