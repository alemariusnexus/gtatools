/*
 * TXDWidget.cpp
 *
 *  Created on: 15.07.2010
 *      Author: alemariusnexus
 */

#include "TXDWidget.h"



TXDWidget::TXDWidget(const File& file, QWidget* parent)
		: QWidget(parent)
{
	ui.setupUi(this);
	ui.mainSplitter->setSizes(QList<int>() << width()/4 << width()/4*3);

	txd = new TXDArchive(file);

	textures = new TXDTexture*[txd->getTextureCount()];

	for (int i = 0 ; i < txd->getTextureCount() ; i++) {
		TXDTexture* texture = txd->nextTexture();
		textures[i] = texture;
		ui.textureList->addItem(texture->getDiffuseName());
	}

	ui.textureCountLabel->setText(QString("%1").arg(txd->getTextureCount()));

	connect(ui.textureList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(textureActivated(QListWidgetItem*)));
}


TXDWidget::~TXDWidget()
{
	for (int i = 0 ; i < txd->getTextureCount() ; i++) {
		delete textures[i];
	}

	delete[] textures;
	delete txd;
}


void TXDWidget::textureActivated(QListWidgetItem* item)
{
	int row = ui.textureList->currentRow();

	if (row >= 0) {
		TXDTexture* texture = textures[row];
		txd->gotoTexture(texture);
		uint8_t* rawData = txd->readTextureData(texture);
		ui.displayLabel->display(texture, rawData);
		delete[] rawData;

		int32_t format = texture->getRasterFormat();
		char formatDesc[16];
		TxdGetRasterFormatName(formatDesc, format);

		ui.formatLabel->setText(formatDesc);
		ui.diffuseNameField->setText(texture->getDiffuseName());
		ui.alphaNameField->setText(texture->getAlphaName());

		TXDCompression compr = texture->getCompression();

		switch (compr) {
		case TXD_COMPRESSION_DXT1:
			ui.compressionField->setText(tr("DXT1"));
			break;
		case TXD_COMPRESSION_DXT3:
			ui.compressionField->setText(tr("DXT3"));
			break;
		case TXD_COMPRESSION_NONE:
			ui.compressionField->setText(tr("None"));
			break;
		}

		ui.dimensionsField->setText(tr("%1x%2").arg(texture->getWidth()).arg(texture->getHeight()));
		ui.bppField->setText(QString("%1").arg(texture->getBytesPerPixel()*8));
		ui.mipmapCountField->setText(QString("%1").arg(texture->getMipmapCount()));
		ui.alphaField->setText(texture->hasAlphaChannel() ? tr("yes") : tr("no"));
	}
}

