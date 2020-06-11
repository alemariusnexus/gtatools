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

#include "TXDWidget.h"
#include <gtatools-gui/config.h>
#include <QSettings>
#include <QLayout>
#include <QTabWidget>
#include "../../System.h"
#include <QString>
#include "TXDFormatHandler.h"
#include <QMessageBox>
#include <gtaformats/txd/TXDException.h>



TXDWidget::TXDWidget(DisplayedFile* dfile, const QString& selectedTex, QWidget* parent)
		: QWidget(parent), dfile(dfile)
{
	System* sys = System::getInstance();

	File file = dfile->getFile();

	ui.setupUi(this);
	ui.mainSplitter->setSizes(QList<int>() << width()/4 << width()/4*3);

	displayWidget = new ImageDisplayWidget(ui.imageDisplayContainerWidget);
	ui.imageDisplayContainerWidget->layout()->addWidget(displayWidget);

	textureTableToolBar = new QToolBar(ui.textureTableContainerWidget);

	textureTableSizeSlider = new QSlider(Qt::Horizontal, textureTableToolBar);
	textureTableSizeSlider->setRange(25, 200);
	connect(textureTableSizeSlider, SIGNAL(valueChanged(int)), this, SLOT(textureTableSizeChanged(int)));

	textureTableToolBar->addWidget(textureTableSizeSlider);

	ui.textureTableToolBarWidget->layout()->addWidget(textureTableToolBar);

#if QT_VERSION >= 0x050000
	ui.textureTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
#else
	ui.textureTable->verticalHeader()->setResizeMode(QHeaderView::Fixed);
#endif

	loadConfigUiSettings();

	extractAction = new QAction(tr("Extract textures..."), NULL);
	connect(extractAction, SIGNAL(triggered(bool)), this, SLOT(textureExtractionRequested(bool)));

	connect(ui.mainSplitter, SIGNAL(splitterMoved(int, int)), this, SLOT(mainSplitterValueChanged(int, int)));

	connect(ui.textureTable, SIGNAL(currentCellChanged(int, int, int, int)), this,
			SLOT(textureActivated(int, int, int, int)));
	connect(ui.textureTable, SIGNAL(customContextMenuRequested(const QPoint&)), this,
			SLOT(textureTableContextMenuRequested(const QPoint&)));
	connect(sys, SIGNAL(configurationChanged()), this, SLOT(loadConfigUiSettings()));

	connect(dfile, SIGNAL(saved()), this, SLOT(reloadHighLevelFile()));

	reloadHighLevelFile();

	openGUIModule = new OpenTXDGUIModule(this);

	sys->installGUIModule(openGUIModule);

	int currentRow = 0;

	if (txd) {
		int i = 0;
		for (TXDArchive::TextureIterator it = txd->getHeaderBegin() ; it != txd->getHeaderEnd() ; it++, i++) {
			TXDTextureHeader* texture = *it;

			if (!selectedTex.isNull()  &&  selectedTex == QString(texture->getDiffuseName().get())) {
				currentRow = i;
				break;
			}
		}
	}

	if (txd  &&  txd->getTextureCount() != 0) {
		ui.textureTable->setCurrentCell(currentRow, 0);
	}
}


TXDWidget::~TXDWidget()
{
	System* sys = System::getInstance();
	sys->uninstallGUIModule(openGUIModule);
	delete openGUIModule;

	if (txd)
		delete txd;
}


void TXDWidget::reloadHighLevelFile()
{
	int crow = ui.textureTable->currentRow();
	QString curTexName;

	if (crow != -1) {
		QTableWidgetItem* curItem = ui.textureTable->item(crow, 0);
		curTexName = curItem->text().toLower();
	}

	try {
		txd = new TXDArchive(dfile->getFile());
	} catch (TXDException ex) {
		QString errmsg = tr("Error opening the TXD file: %1").arg(ex.getMessage().get());
		System::getInstance()->log(LogEntry::error(errmsg, &ex));
		txd = NULL;
		ui.hlWidget->setEnabled(false);
	}

	ui.textureTable->clear();

	int currentRow = -1;

	if (txd) {
		TXDFormatHandler* handler = TXDFormatHandler::getInstance();

		ui.textureTable->setRowCount(txd->getTextureCount());

		int i = 0;
		for (TXDArchive::TextureIterator it = txd->getHeaderBegin() ; it != txd->getHeaderEnd() ; it++, i++) {
			TXDTextureHeader* texture = *it;

			ui.textureTable->setItem(i, 0, new QTableWidgetItem(texture->getDiffuseName().get()));

			uint8_t* rawData = txd->getTextureData(texture);
			uint8_t* convData;
			QImage image = handler->createImageFromTexture(texture, rawData, convData).copy();
			delete[] rawData;
			delete[] convData;
			ImageDisplayLabel* disp = new ImageDisplayLabel(QPixmap::fromImage(image), ui.textureTable);
			disp->setImageAutoScaling(true);
			disp->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.textureTable->setCellWidget(i, 1, disp);

			if (	!curTexName.isNull()
					&&  curTexName.toLower() == QString(texture->getDiffuseName().get()).toLower()
			) {
				currentRow = i;
			}
		}

		ui.textureTable->resizeColumnToContents(0);
	}

	if (txd  &&  txd->getTextureCount() > 0  &&  currentRow != -1)
		ui.textureTable->setCurrentCell(currentRow, 0);
	else
		setDisplayedTexture(NULL);
}


void TXDWidget::loadConfigUiSettings()
{
	QSettings settings;

	ui.mainSplitter->restoreState(settings.value("gui_geometry_TXDWidget/mainSplitter_state").toByteArray());
	textureTableSizeSlider->setValue(settings.value("gui_geometry_TXDWidget/textureTable_rowHeight", 100).toUInt());
}


QLinkedList<TXDTextureHeader*> TXDWidget::getSelectedTextures()
{
	QLinkedList<TXDTextureHeader*> list;

	if (txd) {
		for (int i = 0 ; i < ui.textureTable->rowCount() ; i++) {
			if (ui.textureTable->item(i, 0)->isSelected()) {
				list << txd->getHeader(i);
			}
		}
	}

	return list;
}


void TXDWidget::setDisplayedTexture(TXDTextureHeader* texture)
{
	if (texture) {
		uint8_t* rawData = txd->getTextureData(texture);
		uint8_t* convData;
		QImage image = TXDFormatHandler::getInstance()->createImageFromTexture(texture, rawData, convData);
		delete[] rawData;
		displayWidget->display(image.copy());
		delete[] convData;

		int32_t format = texture->getFullRasterFormat();
		char formatDesc[32];
		TxdGetRasterFormatDescription(formatDesc, format);

		ui.formatLabel->setText(formatDesc);
		ui.diffuseNameField->setText(texture->getDiffuseName().get());
		ui.alphaNameField->setText(texture->getAlphaName().get());

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
		ui.alphaField->setText(texture->isAlphaChannelUsed() ? tr("yes") : tr("no"));

		QString uWrapStr = QString("%1 [").arg(texture->getUWrapFlags());
		QString vWrapStr = QString("%1 [").arg(texture->getVWrapFlags());

		switch (texture->getUWrapFlags()) {
		case WrapClamp:
			uWrapStr.append(tr("Clamp"));
			break;
		case WrapMirror:
			uWrapStr.append(tr("Mirror"));
			break;
		case WrapNone:
			uWrapStr.append(tr("None"));
			break;
		case WrapWrap:
			uWrapStr.append(tr("Wrap"));
			break;
		}

		switch (texture->getVWrapFlags()) {
		case WrapClamp:
			vWrapStr.append(tr("Clamp"));
			break;
		case WrapMirror:
			vWrapStr.append(tr("Mirror"));
			break;
		case WrapNone:
			vWrapStr.append(tr("None"));
			break;
		case WrapWrap:
			vWrapStr.append(tr("Wrap"));
			break;
		}

		uWrapStr.append("]");
		vWrapStr.append("]");

		ui.wrapFlagsLabel->setText(uWrapStr.append("; ").append(vWrapStr));
	} else {
		//ui.displayLabel->display(NULL, NULL);
		ui.formatLabel->setText("-");
		ui.diffuseNameField->setText("-");
		ui.alphaNameField->setText("-");
		ui.compressionField->setText("-");
		ui.dimensionsField->setText("-");
		ui.bppField->setText("-");
		ui.mipmapCountField->setText("-");
		ui.alphaField->setText("-");
		ui.wrapFlagsLabel->setText("-");
	}
}


void TXDWidget::textureActivated(int crow, int ccol, int prow, int pcol)
{
	if (crow >= 0) {
		TXDTextureHeader* texture = txd->getHeader(crow);
		setDisplayedTexture(texture);
	} else {
		setDisplayedTexture(NULL);
	}
}


void TXDWidget::textureTableContextMenuRequested(const QPoint& pos)
{
	QLinkedList<TXDTextureHeader*> texes = getSelectedTextures();

	if (texes.size() > 0) {
		QMenu* menu = new QMenu(ui.textureTable);
		menu->setAttribute(Qt::WA_DeleteOnClose);
		menu->addAction(extractAction);
		menu->popup(ui.textureTable->mapToGlobal(pos));
	}
}


void TXDWidget::textureExtractionRequested(bool checked)
{
	QLinkedList<TXDTextureHeader*> texes = getSelectedTextures();
	TXDFormatHandler::getInstance()->extractTexturesDialog(txd, texes, this);
}


void TXDWidget::textureTableSizeChanged(int size)
{
	QSettings settings;

	settings.setValue("gui_geometry_TXDWidget/textureTable_rowHeight", size);

	ui.textureTable->verticalHeader()->setDefaultSectionSize(size);
}


void TXDWidget::mainSplitterValueChanged(int pos, int idx)
{
	QSettings settings;

	settings.setValue("gui_geometry_TXDWidget/mainSplitter_state", ui.mainSplitter->saveState());
}

