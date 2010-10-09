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

#ifndef GXTTABLEMODEL_H_
#define GXTTABLEMODEL_H_

#include <QAbstractTableModel>
#include <QLinkedList>
#include <gtaformats/gxt/GXTTable.h>



class GXTTableModel : public QAbstractTableModel {
	Q_OBJECT

private:
	struct NamedGXTTable
	{
		GXTTable* table;
		QString name;
	};

public:
	GXTTableModel();
	void addGXTArchive(const QString& name, GXTTable* table);
	virtual int rowCount(const QModelIndex& parent = QModelIndex());
	virtual int columnCount(const QModelIndex& parent = QModelIndex());
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole);

private:
	QLinkedList<NamedGXTTable*> tables;
};

#endif /* GXTTABLEMODEL_H_ */
