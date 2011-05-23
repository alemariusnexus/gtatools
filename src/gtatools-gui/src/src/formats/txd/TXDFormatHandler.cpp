/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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


QWidget* TXDFormatHandler::createWidgetForFile(const FileOpenRequest& request, QWidget* parent)
{
	QString texName;

	QVariant tn = request.getAttribute("texture");
	if (!tn.isNull()) {
		texName = tn.toString();
	}

	return new TXDWidget(*request.getFile(), texName, parent);
}


bool TXDFormatHandler::extractTexturesDialog(TXDArchive* txd, const QLinkedList<TXDTextureHeader*>& texes,
		QWidget* parent)
{
	if (texes.count() == 1) {
		TXDTextureHeader* tex = *texes.begin();

		QString fname = QFileDialog::getSaveFileName(parent, tr("Select the file to save to"),
				QString(tex->getDiffuseName()).append(".png"), "Portable Network Graphics (*.png)");

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

				QImageWriter writer(QString("%1/%2.png").arg(dname).arg(tex->getDiffuseName()));
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
	// Currently does nothing
}
