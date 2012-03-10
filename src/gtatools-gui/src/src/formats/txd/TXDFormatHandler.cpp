/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#include "TXDFormatHandler.h"
#include "TXDGUIModule.h"
#include "TXDWidget.h"
#include <cstdio>
#include <QtCore/QString>
#include <QtGui/QFileDialog>
#include <QtGui/QImage>
#include <QtGui/QImageWriter>
#include "../../System.h"
#include "../../ProfileManager.h"
#include "TextureSearchDialog.h"
#include <gtaformats/txd/TXDConverter.h>
#include "../../DefaultDisplayedFile.h"
#include <gtaformats/util/DefaultFileFinder.h>
#include "TextureFileFinder.h"
#include "../../gui/GUI.h"




TXDFormatHandler* TXDFormatHandler::getInstance()
{
	static TXDFormatHandler* inst = new TXDFormatHandler;
	return inst;
}


TXDFormatHandler::TXDFormatHandler()
{
	connect(System::getInstance(), SIGNAL(systemQuerySent(const SystemQuery&, SystemQueryResult&)), this,
			SLOT(systemQuerySent(const SystemQuery&, SystemQueryResult&)));
}


DisplayedFile* TXDFormatHandler::openFile(const FileOpenRequest& request)
{
	QString texName;

	QVariant tn = request.getAttribute("texture");
	if (!tn.isNull()) {
		texName = tn.toString();
	}

	DefaultDisplayedFile* file = new DefaultDisplayedFile(*request.getFile(), this, NULL);
	TXDWidget* widget;

	try {
		widget = new TXDWidget(file, texName, NULL);
	} catch (Exception& ex) {
		delete file;
		return NULL;
	}

	file->setWidget(widget);

	return file;
}


void TXDFormatHandler::saveFile(DisplayedFile* file, const File& destFile)
{
	TXDWidget* widget = (TXDWidget*) file->getWidget();
	widget->saveTo(destFile);
}


bool TXDFormatHandler::extractTexturesDialog(TXDArchive* txd, const QLinkedList<TXDTextureHeader*>& texes,
		QWidget* parent)
{
	if (texes.count() == 1) {
		TXDTextureHeader* tex = *texes.begin();

		QString fname = QFileDialog::getSaveFileName(parent, tr("Select the file to save to"),
				QString(tex->getDiffuseName().get()).append(".png"), "Portable Network Graphics (*.png)");

		if (!fname.isNull()) {
			//uint8_t* rawData = txd->readTextureData(tex);
			uint8_t* rawData = txd->getTextureData(tex);
			uint8_t* data;
			QImage image = createImageFromTexture(tex, rawData, data);
			image.setText("Description", "Converted from GTA TXD by gtatools " GTATOOLS_VERSION);
			delete[] rawData;

			QImageWriter writer(fname);
			writer.write(image);

			delete[] data;
			return true;
		}

		return false;
	} else {
		QString dname = QFileDialog::getExistingDirectory(parent, tr("Select the destination directory"));

		if (!dname.isNull()) {
			QLinkedList<TXDTextureHeader*>::const_iterator it;

			for (it = texes.begin() ; it != texes.end() ; it++) {
				TXDTextureHeader* tex = *it;
				uint8_t* rawData = txd->getTextureData(tex);
				uint8_t* data;
				QImage image = createImageFromTexture(tex, rawData, data);
				image.setText("Description", "Converted from GTA TXD by gtatools " GTATOOLS_VERSION);
				delete[] rawData;

				QImageWriter writer(QString("%1/%2.png").arg(dname).arg(tex->getDiffuseName().get()));
				writer.write(image);

				delete[] data;
			}

			return true;
		}

		return false;
	}
}


bool TXDFormatHandler::findTextureDialog(const QLinkedList<File*>& files, QWidget* parent)
{
	TextureSearchDialog dlg(parent, files);
	return dlg.exec() == QDialog::Accepted;
}


QImage TXDFormatHandler::createImageFromTexture(TXDTextureHeader* tex, uint8_t* data, uint8_t*& resultData)
{
	TXDConverter conv;

	TXDTextureHeader toTex(*tex);
	toTex.setRasterFormat(RasterFormatB8G8R8A8);

	//if (tex->canConvert()) {
	if (conv.canConvert(*tex, toTex)) {
		int16_t w = tex->getWidth();
		int16_t h = tex->getHeight();

		resultData = new uint8_t[w*h*4];

		conv.convert(*tex, toTex, data, resultData, 0, 0);

		QImage image(resultData, w, h, QImage::Format_ARGB32);

		return image;
	} else {
		resultData = NULL;
		return System::getInstance()->getDummyTextureImage();
	}
}


void TXDFormatHandler::systemQuerySent(const SystemQuery& query, SystemQueryResult& result)
{
	if (!result.isSuccessful()) {
		if (query.getName() == "FindAndOpenTexture") {
			QString tex = query["texture"].toString();
			QString txdName = query["txdName"].toString();

			QString txdFileName = QString(txdName).append(".txd");

			FileFinder* txdFinder = NULL;

			if (!txdName.isNull())
				txdFinder = new DefaultFileFinder(txdFileName.toAscii().constData(), false);

			DefaultFileFinder texFinder(tex.toAscii().constData(), false);

			TextureFileFinder finder(&texFinder, txdFinder);

			File* file = GUI::getInstance()->findFile(&finder, NULL);

			if (file) {
				result["txdFile"] = QString(file->getPath()->toString());

				System* sys = System::getInstance();
				FileOpenRequest req(*file);
				req.setAttribute("texture", finder.getMatchedTexture(*file));
				delete file;
				sys->openFile(req);

				result.setSuccessful(true);
			}
		}
	}
}
