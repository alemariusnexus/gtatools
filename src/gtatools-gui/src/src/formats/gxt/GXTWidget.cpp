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

#include <config.h>
#include "GXTWidget.h"
#include <gtaformats/gxt/GXTLoader.h>
#include <gtaformats/gxt/GXTTable.h>
#include <gtaformats/gta.h>
#include <gtaformats/util/util.h>
#include <QtCore/QSettings>
#include <QtGui/QMessageBox>
#include <cstdio>
#include "../../System.h"



GXTWidget::GXTWidget(QWidget* parent)
		: QWidget(parent), model(NULL), guiModule(this)
{
	ui.setupUi(this);

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

	for (it = gxtFiles.begin() ; it != gxtFiles.end() ; it++) {
		delete *it;
	}

	QMap<QString, QVector<GXTTable*> >::iterator tit;

	for (tit = tables.begin() ; tit != tables.end() ; tit++) {
		QVector<GXTTable*>& langTables = tit.value();

		QVector<GXTTable*>::iterator lit;

		for (lit = langTables.begin() ; lit != langTables.end() ; lit++) {
			delete *lit;
		}
	}
}


void GXTWidget::addFile(const File& file)
{
	gxtFiles << new File(file);
	GXTLoader gxt(file);
	gxt.setEncoding(UTF8);
	gxt.setKeepKeyNames(true);
	GXTTableHeader* headers = new GXTTableHeader[gxt.getTableCount()];
	gxt.readTableHeaders(headers);

	for (int32_t i = 0 ; i < gxt.getTableCount() ; i++) {
		GXTTable* table = gxt.readTableData(headers[i]);

		QVector<GXTTable*>& langTables = tables[headers[i].name];
		langTables.resize(gxtFiles.size());
		langTables[langTables.size()-1] = table;
	}

	rebuildTableList();
}


void GXTWidget::stringListMatch(const File& file)
{
	InputStream* stream = file.openStream();

	QVector<GXTTable*>* langTables = NULL;

	char buffer[4096];
	while (!stream->hasReachedEnd()) {
		stream->readLine(buffer, sizeof(buffer));

		QString line(buffer);
		int cmtIndex = line.indexOf("//");

		if (cmtIndex != -1) {
			line = line.left(cmtIndex);
		}

		line = line.trimmed();

		if (line.isEmpty()) {
			continue;
		}

		if (line.startsWith('[')  &&  line.endsWith(']')) {
			QString tableName = line.mid(1, line.length()-2);
			langTables = &tables[tableName];
		} else {
			if (!langTables) {
				QMessageBox::critical(this, tr("Syntax Error"),
						tr("Syntax Error: Key name definition is not in a table!"));
				return;
			}

			QStringList kv = line.split('=');
			crc32_t keyHash;
			QString keyName;

			if (kv.size() == 1) {
				keyName = line;
				keyHash = Crc32(line.toAscii().constData());
			} else {
				keyHash = kv[0].toUInt(NULL, 16);
				keyName = kv[1];
			}

			QVector<GXTTable*>::iterator it;

			for (it = langTables->begin() ; it != langTables->end() ; it++) {
				GXTTable* table = *it;
				table->setKeyName(keyHash, keyName.toAscii().constData());
			}
		}
	}

	delete stream;

	if (ui.tableList->currentRow() != -1) {
		displayTable(ui.tableList->currentItem()->text());
	}
}


void GXTWidget::rebuildTableList()
{
	QMap<QString, QVector<GXTTable*> >::iterator it;

	for (it = tables.begin() ; it != tables.end() ; it++) {
		ui.tableList->addItem(it.key());
	}
}


void GXTWidget::displayTable(const QString& table)
{
	if (model) {
		delete model;
	}

	ui.tableLabel->setText(table);

	model = new GXTTableModel;

	QLinkedList<File*>::iterator it;
	QVector<GXTTable*>& langTables = tables[table];

	for (it = gxtFiles.begin() ; it != gxtFiles.end() ; it++) {
		File* file = *it;

		QVector<GXTTable*>::iterator tit;

		for (tit = langTables.begin() ; tit != langTables.end() ; tit++) {
			model->addGXTArchive(file->getPath()->getFileName(), *tit);
		}
	}

	proxyModel->setSourceModel(model);
	ui.table->setModel(proxyModel);

	QHeaderView* header = ui.table->horizontalHeader();
	header->setResizeMode(QHeaderView::Stretch);
	header->setResizeMode(0, QHeaderView::ResizeToContents);
}


void GXTWidget::applyFilter()
{
	QString filter = ui.filterField->text();

	if (ui.hashBox->isChecked()) {
		filter = QString("0x%1").arg((uint32_t) Crc32(filter.toAscii().constData()), 0, 16);
	}

	if (ui.regexBox->isChecked()) {
		QSettings settings(CONFIG_FILE, QSettings::IniFormat);

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


void GXTWidget::currentTableChanged(int row)
{
	QMap<QString, QVector<GXTTable*> >::iterator it = tables.begin() + row;
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
