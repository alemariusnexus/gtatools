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

#include "GXTTableModel.h"




GXTTableModel::GXTTableModel()
		: QAbstractTableModel()
{
}


void GXTTableModel::addLanguageTable(const QString& name, GXTTable* table)
{
	langTables[name] = table;
	GXTTable::EntryIterator it;

	for (it = table->getFirstEntry() ; it != table->getLastEntry() ; it++) {
		if (!keyHashes.contains(it->first)) {
			keyHashes << it->first;
		}
	}
}


int GXTTableModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid()) {
		return 0;
	} else {
		return keyHashes.size();
	}
}


int GXTTableModel::columnCount(const QModelIndex& parent) const
{
	if (parent.isValid()) {
		return 0;
	} else {
		return langTables.size()+1;
	}
}


QVariant GXTTableModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid()  ||  role != Qt::DisplayRole) {
		return QVariant();
	}

	int row = index.row();
	int col = index.column();

	crc32_t hash = keyHashes[row];

	if (col == 0) {
		QMap<QString, GXTTable*>::const_iterator it;

		QString hashStr = QString("%1").arg((uint32_t) hash, 8, 16, QLatin1Char('0'));
		hashStr = hashStr.toUpper();

		for (it = langTables.begin() ; it != langTables.end() ; it++) {
			GXTTable* table = it.value();
			const char* keyName = table->getKeyName(hash).get();

			if (keyName) {
				return QVariant(QString("%1 [0x%2]").arg(keyName).arg(hashStr));
			}
		}

		return QVariant(QString("[0x%1]").arg(hashStr));
	} else {
		GXTTable* table = (langTables.begin() + (col-1)).value();

		char* value = table->getValue(hash);

		if (value) {
			return QVariant(QString::fromUtf8(value));
		} else {
			return QVariant(tr("[Not assigned]"));
		}
	}
}


QVariant GXTTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (role != Qt::DisplayRole  ||  orientation != Qt::Horizontal) {
		return QVariant();
	}

	if (section == 0) {
		return tr("Key");
	}

	return (langTables.begin()+section-1).key();
}
