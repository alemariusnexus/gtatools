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

#ifndef GXTWIDGET_H_
#define GXTWIDGET_H_

#include <QtGui/QWidget>
#include <QtCore/QMap>
#include <QtCore/QLinkedList>
#include <QtCore/QVector>
#include <QtGui/QSortFilterProxyModel>
#include <ui_GXTWidget.h>
#include <gtaformats/util/File.h>
#include "GXTTableModel.h"
#include "GXTGUIModule.h"
#include <QtCore/QSettings>


class GXTWidget : public QWidget {
	Q_OBJECT

public:
	GXTWidget(QWidget* parent = NULL);
	~GXTWidget();
	void addFile(const File& file);
	void stringListMatch(const File& file);

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
	QLinkedList<File*> gxtFiles;
	QMap<QString, QVector<GXTTable*> > tables;
};

#endif /* GXTWIDGET_H_ */
