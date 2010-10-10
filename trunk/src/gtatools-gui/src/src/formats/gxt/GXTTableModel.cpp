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

#include "GXTTableModel.h"




GXTTableModel::GXTTableModel()
		: QAbstractTableModel()
{
}


void GXTTableModel::addGXTArchive(const QString& name, GXTTable* table)
{
	NamedGXTTable* ntable = new NamedGXTTable;
	ntable->table = table;
	ntable->name = name;
	tables << ntable;
}


int GXTTableModel::rowCount(const QModelIndex& parent)
{
	if (parent.isValid()) {
		return 0;
	} else {
		QLinkedList<NamedGXTTable*>::iterator it;
		int count = 0;

		for (it = tables.begin() ; it != tables.end() ; it++) {
			NamedGXTTable* table = *it;

			if (table->table->getEntryCount() > count) {
				count = table->table->getEntryCount();
			}
		}

		return count;
	}
}


int GXTTableModel::columnCount(const QModelIndex& parent)
{
	if (parent.isValid()) {
		return 0;
	} else {
		return tables.size()+1;
	}
}


QVariant GXTTableModel::data(const QModelIndex& index, int role)
{
	if (!index.isValid()) {
		return QVariant();
	}

	int row = index.row();
	int col = index.column();

	NamedGXTTable* ntable = tables.first();
	GXTTable* table = ntable->table;

	if (col == 0) {
		GXTTable::EntryIterator it = table->getFirstEntry() + row;

	}
}
