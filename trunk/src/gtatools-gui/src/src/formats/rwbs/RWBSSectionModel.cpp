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

#include "RWBSSectionModel.h"
#include <gtaformats/gta.h>



void RWBSSectionModel::addRootSection(RWSection* sect)
{
	rootSects << sect;
}


Qt::ItemFlags RWBSSectionModel::flags(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return 0;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant RWBSSectionModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}

	RWSection* sect = (RWSection*) index.internalPointer();

	if (role == Qt::DisplayRole) {
		if (index.column() == 0) {
			char sectName[64];
			RwGetSectionName(sect->getID(), sectName);
			return QString(tr("%1 [0x%2, %3 bytes @ 0x%4]"))
					.arg((strncmp(sectName, "RW_", 3) == 0) ? sectName+11 : sectName) // Without RW_SECTION_
					.arg(QString("%1").arg(sect->getID(), 0, 16, QChar('0')).toUpper())
					.arg(sect->getSize())
					.arg(QString("%1").arg(sect->getAbsoluteOffset(), 0, 16).toUpper());
		}
	}

	return QVariant();
}


QVariant RWBSSectionModel::headerData(int section, Qt::Orientation orient, int role) const
{
	return QVariant();
}


int RWBSSectionModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid()) {
		if (parent.column() == 0) {
			RWSection* sect = (RWSection*) parent.internalPointer();
			return sect->getChildCount();
		}
	} else {
		return rootSects.size();
	}

	return 0;
}


int RWBSSectionModel::columnCount(const QModelIndex& parent) const
{
	return 1;
}


QModelIndex RWBSSectionModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	if (parent.isValid()) {
		RWSection* sect = (RWSection*) parent.internalPointer();
		return createIndex(row, column, *(sect->getChildBegin() + row));
	} else {
		return createIndex(row, column, rootSects[row]);
	}
}


QModelIndex RWBSSectionModel::parent(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return QModelIndex();
	}

	RWSection* sect = (RWSection*) index.internalPointer();
	RWSection* parentSect = sect->getParent();

	if (parentSect) {
		RWSection* parentParentSect = parentSect->getParent();

		if (parentParentSect) {
			return createIndex(parentParentSect->indexOf(parentSect), 0, parentSect);
		} else {
			return createIndex(rootSects.indexOf(parentSect), 0, parentSect);
		}
	} else {
		return QModelIndex();
	}
}
