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

#include "OpenTXDGUIModule.h"
#include "TXDWidget.h"
#include <cstdio>
#include "../../gui/MainWindow.h"
#include <gtaformats/txd/TXDArchive.h>
#include <gtaformats/txd/TXDTexture.h>
#include <gtaformats/gf_config.h>
#include <qlinkedlist.h>
#include <qfiledialog.h>
#include <qimagewriter.h>
#include "TXDFormatHandler.h"
#include "../../System.h"




OpenTXDGUIModule::OpenTXDGUIModule(TXDWidget* txdWidget)
		: txdWidget(txdWidget)
{
	extractAction = new QAction(tr("Extract textures..."), NULL);
	extractAction->setShortcut(Qt::CTRL + Qt::Key_E);
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
	QLinkedList<TXDTexture*> textures = txdWidget->getSelectedTextures();

	System* sys = System::getInstance();

	if (textures.count() == 1) {
		TXDTexture* tex = *textures.begin();

		QString fname = QFileDialog::getSaveFileName(txdWidget, tr("Select the file to save to"),
				QString(tex->getDiffuseName()).append(".png"), "Portable Network Graphics (*.png)");

		if (!fname.isNull()) {
			txd->gotoTexture(tex);
			uint8_t* rawData = txd->readTextureData(tex);
			uint8_t* data;
			QImage image = TXDFormatHandler::getInstance()->createImageFromTexture(tex, rawData, data);
			image.setText("Description", "Converted from GTA TXD by gtatools " GTATOOLS_VERSION);
			delete[] rawData;

			QImageWriter writer(fname);
			writer.write(image);

			delete[] data;
		}
	} else if (textures.count() > 1) {
		QString dname = QFileDialog::getExistingDirectory(txdWidget, tr("Select the destination directory"));

		if (!dname.isNull()) {
			QLinkedList<TXDTexture*>::iterator it;

			for (it = textures.begin() ; it != textures.end() ; it++) {
				TXDTexture* tex = *it;
				txd->gotoTexture(tex);
				uint8_t* rawData = txd->readTextureData(tex);
				uint8_t* data;
				QImage image = TXDFormatHandler::getInstance()->createImageFromTexture(tex, rawData, data);
				delete[] rawData;

				QImageWriter writer(QString("%1/%2.png").arg(dname).arg(tex->getDiffuseName()));
				writer.write(image);

				delete[] data;
			}
		}
	}
}

