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

#include "TXDWidget.h"
#include "../../config.h"
#include <QtCore/QSettings>
#include <QtGui/QLayout>
#include <QtGui/QTabWidget>
#include "../../System.h"
#include <QtCore/QString>
#include "TXDFormatHandler.h"
#include <QtGui/QMessageBox>
#include <gtaformats/txd/TXDException.h>



TXDWidget::TXDWidget(const File& file, const QString& selectedTex, QWidget* parent)
		: QWidget(parent), compactTab(NULL)
{
	ui.setupUi(this);
	ui.mainSplitter->setSizes(QList<int>() << width()/4 << width()/4*3);


	QWidget* displayContainer = ui.renderArea->takeWidget();
	displayContainer->layout()->removeWidget(ui.displayLabel);
	ui.renderArea->setWidget(ui.displayLabel);
	delete displayContainer;

	ui.renderArea->viewport()->setMouseTracking(true);

	loadConfigUiSettings();

	try {
		txd = new TXDArchive(file);
	} catch (TXDException ex) {
		System::getInstance()->logError(ex.what());
		QMessageBox::critical(this, tr("Error opening TXD file"), tr("The following error occurred "
				"opening the TXD file:\n\n%1\n\nSee the error log fore more details.").arg(ex.getMessage()));
		throw;
	}

	int currentRow = 0;

	int i = 0;
	for (TXDArchive::TextureIterator it = txd->getHeaderBegin() ; it != txd->getHeaderEnd() ; it++, i++) {
		TXDTextureHeader* texture = *it;
		ui.textureList->addItem(texture->getDiffuseName());

		if (!selectedTex.isNull()  &&  selectedTex == QString(texture->getDiffuseName())) {
			currentRow = i;
		}
	}

	extractAction = new QAction(tr("Extract textures..."), NULL);
	connect(extractAction, SIGNAL(triggered(bool)), this, SLOT(textureExtractionRequested(bool)));

	System* sys = System::getInstance();

	connect(ui.textureList, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this,
			SLOT(textureActivated(QListWidgetItem*, QListWidgetItem*)));
	connect(ui.textureList, SIGNAL(customContextMenuRequested(const QPoint&)), this,
			SLOT(textureListContextMenuRequested(const QPoint&)));
	connect(sys, SIGNAL(configurationChanged()), this, SLOT(loadConfigUiSettings()));

	openGUIModule = new OpenTXDGUIModule(this);

	sys->installGUIModule(openGUIModule);

	if (txd->getTextureCount() != 0) {
		ui.textureList->setCurrentRow(currentRow);
	}
}


TXDWidget::~TXDWidget()
{
	if (compactTab) {
		delete compactTab;
	}

	System* sys = System::getInstance();
	sys->uninstallGUIModule(openGUIModule);
	delete openGUIModule;

	delete txd;
}


void TXDWidget::loadConfigUiSettings()
{
	QSettings settings;

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


QLinkedList<TXDTextureHeader*> TXDWidget::getSelectedTextures()
{
	QLinkedList<TXDTextureHeader*> list;

	for (int i = 0 ; i < ui.textureList->count() ; i++) {
		if (ui.textureList->item(i)->isSelected()) {
			list << txd->getHeader(i);
		}
	}

	return list;
}


void TXDWidget::textureActivated(QListWidgetItem* item, QListWidgetItem* previous)
{
	int row = ui.textureList->currentRow();

	if (row >= 0) {
		TXDTextureHeader* texture = txd->getHeader(row);
		uint8_t* rawData = txd->getTextureData(texture);
		ui.displayLabel->display(texture, rawData);
		delete[] rawData;

		int32_t format = texture->getRasterFormat();
		char formatDesc[32];
		TxdGetRasterFormatDescription(formatDesc, format);

		ui.formatLabel->setText(formatDesc);
		ui.diffuseNameField->setText(texture->getDiffuseName());
		ui.alphaNameField->setText(texture->getAlphaName());

		TXDCompression compr = texture->getCompression();

		switch (compr) {
		case DXT1:
			ui.compressionField->setText(tr("DXT1"));
			break;
		case DXT3:
			ui.compressionField->setText(tr("DXT3"));
			break;
		case PVRTC2:
			ui.compressionField->setText(tr("PVRTC2"));
			break;
		case PVRTC4:
			ui.compressionField->setText(tr("PVRTC4"));
			break;
		case NONE:
			ui.compressionField->setText(tr("None"));
			break;
		}

		ui.dimensionsField->setText(tr("%1x%2").arg(texture->getWidth()).arg(texture->getHeight()));
		ui.bppField->setText(QString("%1").arg(texture->getBytesPerPixel()*8));
		ui.mipmapCountField->setText(QString("%1").arg(texture->getMipmapCount()));
		ui.alphaField->setText(texture->hasAlphaChannel() ? tr("yes") : tr("no"));
	}
}


void TXDWidget::textureListContextMenuRequested(const QPoint& pos)
{
	QLinkedList<TXDTextureHeader*> texes = getSelectedTextures();

	if (texes.size() > 0) {
		QMenu* menu = new QMenu(ui.textureList);
		menu->setAttribute(Qt::WA_DeleteOnClose);
		menu->addAction(extractAction);
		menu->popup(ui.textureList->mapToGlobal(pos));
	}
}


void TXDWidget::textureExtractionRequested(bool checked)
{
	QLinkedList<TXDTextureHeader*> texes = getSelectedTextures();
	TXDFormatHandler::getInstance()->extractTexturesDialog(txd, texes, this);
}

