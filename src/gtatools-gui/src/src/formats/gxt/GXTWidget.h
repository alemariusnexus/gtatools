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

#ifndef GXTWIDGET_H_
#define GXTWIDGET_H_

#include <QWidget>
#include <QMap>
#include <QLinkedList>
#include <QVector>
#include <QSortFilterProxyModel>
#include <ui_GXTWidget.h>
#include <nxcommon/file/File.h>
#include "GXTTableModel.h"
#include "GXTGUIModule.h"
#include <QSettings>
#include "../../EntityOpenRequest.h"


class GXTWidget : public QWidget {
	Q_OBJECT

public:
	GXTWidget(const EntityOpenRequest& request, QWidget* parent = NULL);
	~GXTWidget();
	QMap<QString, GXTTable*> getTables() { return tables; }
	QLinkedList<GXTTable*> getSelectedTables();
	File* getOpenFile() { return gxtFile; }
	void reloadCurrentTable();

private:
	void rebuildTableList();
	void displayTable(const QString& table);
	void applyFilter();

private slots:
	void currentTableChanged(int row);
	void filterChanged(const QString& filter);
	void filterRegexStateChanged(int state);
	void filterHashStateChanged(int state);

private:
	Ui_GXTWidget ui;
	GXTGUIModule guiModule;
	GXTTableModel* model;
	QSortFilterProxyModel* proxyModel;
	File* gxtFile;
	QMap<QString, GXTTable*> tables;
};

#endif /* GXTWIDGET_H_ */
