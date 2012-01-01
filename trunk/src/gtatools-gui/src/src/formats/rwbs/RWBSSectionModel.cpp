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

#include "RWBSSectionModel.h"
#include <gtaformats/gta.h>



void RWBSSectionModel::addRootSection(RWSection* sect)
{
	rootSects << sect;
}


void RWBSSectionModel::removeRootSection(RWSection* sect)
{
	rootSects.removeOne(sect);
}


void RWBSSectionModel::insertSection(RWSection* sect, RWSection* parent, int index)
{
	QModelIndex parentIdx = getSectionIndex(parent);

	if (index == -1) {
		if (parent)
			index = parent->getChildCount();
		else
			index = rootSects.count();
	}

	beginInsertRows(parentIdx, index, index);

	if (parent)
		parent->insertChild(parent->getChildBegin() + index, sect);
	else
		rootSects.insert(index, sect);

	endInsertRows();

	emit sectionInserted(sect);
}


int RWBSSectionModel::removeSection(RWSection* sect)
{
	RWSection* parent = sect->getParent();

	int index;

	if (parent)
		index = parent->indexOf(sect);
	else
		index = rootSects.indexOf(sect);

	QModelIndex parentIdx = getSectionIndex(parent);

	beginRemoveRows(parentIdx, index, index);

	if (parent)
		parent->removeChild(sect);
	else
		rootSects.removeOne(sect);

	endRemoveRows();

	emit sectionRemoved(sect, parent);

	return index;
}


QModelIndex RWBSSectionModel::getSectionIndex(RWSection* sect)
{
	if (!sect)
		return QModelIndex();

	RWSection* parent = sect->getParent();

	if (parent) {
		QModelIndex parentIdx = getSectionIndex(parent);
		QModelIndex idx = index(parent->indexOf(sect), 0, parentIdx);
		return idx;
	} else {
		QModelIndex idx = index(rootSects.indexOf(sect), 0);
		return idx;
	}
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
			char sectName[RW_MAX_SECTION_NAME];
			RWGetSectionName(sect->getID(), sectName);
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
			return sect->isDataSection() ? 0 : sect->getChildCount();
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
		QModelIndex index = createIndex(row, column, *(sect->getChildBegin() + row));
		return index;
	} else {
		QModelIndex index = createIndex(row, column, rootSects[row]);
		return index;
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
