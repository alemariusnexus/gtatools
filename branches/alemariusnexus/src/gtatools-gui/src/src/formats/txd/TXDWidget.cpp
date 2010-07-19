/*
 * TXDWidget.cpp
 *
 *  Created on: 15.07.2010
 *      Author: alemariusnexus
 */

#include "TXDWidget.h"
#include <qsettings.h>
#include <config.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include "../../System.h"



TXDWidget::TXDWidget(const File& file, QWidget* parent)
		: QWidget(parent), compactTab(NULL)
{
	ui.setupUi(this);
	ui.mainSplitter->setSizes(QList<int>() << width()/4 << width()/4*3);

	loadConfigUiSettings();

	txd = new TXDArchive(file);

	textures = new TXDTexture*[txd->getTextureCount()];

	for (int i = 0 ; i < txd->getTextureCount() ; i++) {
		TXDTexture* texture = txd->nextTexture();
		textures[i] = texture;
		ui.textureList->addItem(texture->getDiffuseName());
	}

	ui.textureCountLabel->setText(QString("%1").arg(txd->getTextureCount()));

	System* sys = System::getInstance();

	connect(sys, SIGNAL(configurationChanged()), this, SLOT(configurationChanged()));
	connect(ui.textureList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(textureActivated(QListWidgetItem*)));

	openGUIModule = new OpenTXDGUIModule(this);

	sys->installGUIModule(openGUIModule);
}


TXDWidget::~TXDWidget()
{
	System* sys = System::getInstance();
	sys->uninstallGUIModule(openGUIModule);
	delete openGUIModule;

	for (int i = 0 ; i < txd->getTextureCount() ; i++) {
		delete textures[i];
	}

	delete[] textures;
	delete txd;
}


void TXDWidget::loadConfigUiSettings()
{
	QSettings settings(CONFIG_FILE, QSettings::IniFormat);

	if (settings.value("gui/compact_mode", false).toBool()) {
		QLayout* layout = ui.displayWidget->layout();
		layout->removeWidget(ui.infoWidget);
		layout->removeWidget(ui.renderArea);

		QTabWidget* tw = new QTabWidget;
		tw->addTab(ui.infoWidget, tr("&Information"));
		tw->addTab(ui.renderArea, tr("&Display"));
		layout->addWidget(tw);

		compactTab = tw;
	} else {
		if (compactTab) {
			QLayout* layout = ui.displayWidget->layout();
			layout->removeWidget(compactTab);
			compactTab->removeTab(0);
			compactTab->removeTab(1);
			ui.infoWidget->setParent(ui.displayWidget);
			ui.renderArea->setParent(ui.displayWidget);
			delete compactTab;
			layout->addWidget(ui.infoWidget);
			layout->addWidget(ui.renderArea);
			ui.infoWidget->show();
			ui.renderArea->show();
		}

		compactTab = NULL;
	}
}


QLinkedList<TXDTexture*> TXDWidget::getSelectedTextures()
{
	QLinkedList<TXDTexture*> list;

	for (int i = 0 ; i < ui.textureList->count() ; i++) {
		if (ui.textureList->item(i)->isSelected()) {
			list << textures[i];
		}
	}

	return list;
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


void TXDWidget::configurationChanged()
{
	loadConfigUiSettings();
}

