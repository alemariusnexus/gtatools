/*
 * GXTTableModel.cpp
 *
 *  Created on: 26.09.2010
 *      Author: alemariusnexus
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
