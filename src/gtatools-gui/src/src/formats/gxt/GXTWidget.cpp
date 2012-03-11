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

#include <gtatools-gui/config.h>
#include "GXTWidget.h"
#include <gtaformats/gxt/GXTLoader.h>
#include <gtaformats/gxt/GXTTable.h>
#include <gtaformats/gta.h>
#include <gtaformats/util/util.h>
#include <QtCore/QSettings>
#include <QtGui/QMessageBox>
#include <cstdio>
#include "../../System.h"
#include "GXTFormatHandler.h"



GXTWidget::GXTWidget(const FileOpenRequest& request, QWidget* parent)
		: QWidget(parent), guiModule(this), model(NULL)
{
	ui.setupUi(this);

	File* file = request.getFile();

	gxtFile = new File(*file);
	GXTLoader gxt(*file);
	gxt.setEncoding(UTF8);
	gxt.setKeepKeyNames(true);
	GXTTableHeader* headers = new GXTTableHeader[gxt.getTableCount()];
	gxt.readTableHeaders(headers);

	for (int32_t i = 0 ; i < gxt.getTableCount() ; i++) {
		GXTTable* table = gxt.readTableData(headers[i]);
		tables[table->getName().get()] = table;
	}

	rebuildTableList();

	connect(ui.tableList, SIGNAL(currentRowChanged(int)), this, SLOT(currentTableChanged(int)));
	connect(ui.filterField, SIGNAL(textChanged(const QString&)), this, SLOT(filterChanged(const QString&)));
	connect(ui.regexBox, SIGNAL(stateChanged(int)), this, SLOT(filterRegexStateChanged(int)));
	connect(ui.hashBox, SIGNAL(stateChanged(int)), this, SLOT(filterHashStateChanged(int)));

	proxyModel = new QSortFilterProxyModel;
	proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
	proxyModel->setFilterKeyColumn(-1);

	System::getInstance()->installGUIModule(&guiModule);
}


GXTWidget::~GXTWidget()
{
	System::getInstance()->uninstallGUIModule(&guiModule);

	if (model) {
		delete model;
	}

	QLinkedList<File*>::iterator it;

	delete gxtFile;

	QMap<QString, GXTTable*>::iterator tit;

	for (tit = tables.begin() ; tit != tables.end() ; tit++) {
		delete tit.value();
	}

	/*QMap<QString, QVector<GXTTable*> >::iterator tit;

	for (tit = tables.begin() ; tit != tables.end() ; tit++) {
		QVector<GXTTable*>& langTables = tit.value();

		QVector<GXTTable*>::iterator lit;

		for (lit = langTables.begin() ; lit != langTables.end() ; lit++) {
			delete *lit;
		}
	}*/
}


void GXTWidget::rebuildTableList()
{
	QMap<QString, GXTTable*>::iterator it;

	for (it = tables.begin() ; it != tables.end() ; it++) {
		ui.tableList->addItem(it.key());
	}
}


void GXTWidget::displayTable(const QString& tablename)
{
	if (model) {
		delete model;
	}

	ui.tableLabel->setText(tablename);

	model = new GXTTableModel;

	GXTTable* table = tables[tablename];
	model->addLanguageTable(gxtFile->getPath()->getFileName().get(), table);

	proxyModel->setSourceModel(model);
	ui.table->setModel(proxyModel);

	QHeaderView* header = ui.table->horizontalHeader();
	header->setResizeMode(QHeaderView::Stretch);
	header->setResizeMode(0, QHeaderView::ResizeToContents);
}


QLinkedList<GXTTable*> GXTWidget::getSelectedTables()
{
	QLinkedList<GXTTable*> tableList;

	QList<QListWidgetItem*> selection = ui.tableList->selectedItems();
	for (int i = 0 ; i < selection.size() ; i++) {
		int row = ui.tableList->row(selection[i]);
		GXTTable* table = (tables.begin() + row).value();
		tableList << table;
	}

	return tableList;
}


void GXTWidget::applyFilter()
{
	QString filter = ui.filterField->text();

	if (ui.hashBox->isChecked()) {
		filter = QString("0x%1").arg((uint32_t) Crc32(filter.toAscii().constData()), 0, 16);
	}

	if (ui.regexBox->isChecked()) {
		QSettings settings;

		QRegExp::PatternSyntax syntax;
		QString syntaxSetting = settings.value("main/regex_syntax", "wildcard").toString();

		if (syntaxSetting == "wildcard") {
			syntax = QRegExp::WildcardUnix;
		} else if (syntaxSetting == "full") {
			syntax = QRegExp::RegExp;
		} else {
			syntax = QRegExp::WildcardUnix;
		}

		QRegExp regex(filter, Qt::CaseInsensitive, syntax);
		proxyModel->setFilterRegExp(regex);
	} else {
		proxyModel->setFilterFixedString(filter);
	}
}


void GXTWidget::reloadCurrentTable()
{
	if (ui.tableList->currentRow() != -1) {
		displayTable(ui.tableList->currentItem()->text());
	}
}


void GXTWidget::currentTableChanged(int row)
{
	QMap<QString, GXTTable*>::iterator it = tables.begin() + row;
	displayTable(it.key());
}


void GXTWidget::filterChanged(const QString& filter)
{
	applyFilter();
}


void GXTWidget::filterRegexStateChanged(int state)
{
	applyFilter();
}


void GXTWidget::filterHashStateChanged(int state)
{
	applyFilter();
}
