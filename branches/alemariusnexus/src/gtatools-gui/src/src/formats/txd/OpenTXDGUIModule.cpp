/*
 * OpenTXDGUIModule.cpp
 *
 *  Created on: 18.07.2010
 *      Author: alemariusnexus
 */

#include "OpenTXDGUIModule.h"
#include "TXDWidget.h"
#include <cstdio>
#include "../../gui/MainWindow.h"
#include <gtaformats/txd/TXDArchive.h>
#include <gtaformats/txd/TXDTexture.h>
#include <gf_config.h>
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

		QString fname = QFileDialog::getSaveFileName(txdWidget, tr("Select the file to save to"), QString(),
				"Portable Network Graphics (*.png)");

		if (!fname.isNull()) {
			txd->gotoTexture(tex);
			uint8_t* rawData = txd->readTextureData(tex);
			uint8_t* data;
			QImage image = TXDFormatHandler::getInstance()->createImageFromTexture(tex, rawData, data);
			delete[] rawData;

			QImageWriter writer(fname);
			writer.write(image);

			delete[] data;

			sys->showStatusMessage(tr("Extraction successful!"));
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

			sys->showStatusMessage(tr("Extraction successful!"));
		}
	}
}

