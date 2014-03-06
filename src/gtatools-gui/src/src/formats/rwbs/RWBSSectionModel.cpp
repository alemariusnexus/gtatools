/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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
	insertSection(sect, NULL);
}


void RWBSSectionModel::removeRootSection(RWSection* sect)
{
	removeSection(sect);
}


QUndoCommand* RWBSSectionModel::createInsertSectionCommand(RWSection* sect, RWSection* parent, int index)
{
	if (index == -1) {
		if (parent)
			index = parent->getChildCount();
		else
			index = rootSects.count();
	}

	return new RWBSSectionInsertCommand(this, parent, sect, index);
}


QUndoCommand* RWBSSectionModel::createRemoveSectionCommand(RWSection* sect)
{
	return new RWBSSectionRemoveCommand(this, sect->getParent(),
			sect->getParent() ? sect->getParent()->indexOf(sect) : getRootSectionIndex(sect));
}


QUndoCommand* RWBSSectionModel::createClearCommand()
{
	CompoundUndoCommand* cmd = new CompoundUndoCommand;

	for (SectIterator it = rootSects.begin() ; it != rootSects.end() ; it++) {
		QUndoCommand* rcmd = createRemoveSectionCommand(*it);
		cmd->addCommand(rcmd);
	}

	return cmd;
}


QUndoCommand* RWBSSectionModel::createUpdateSectionCommand(RWSection* sect, uint32_t id, uint32_t version, bool container)
{
	return new RWBSSectionUpdateCommand(this, sect, id, version, container);
}


void RWBSSectionModel::updateSection(RWSection* sect, uint32_t id, uint32_t version, bool container)
{
	uint32_t oldId = sect->getID();
	uint32_t oldVer = sect->getVersion();
	bool oldCont = sect->isContainerSection();

	emit layoutAboutToBeChanged();

	QModelIndex oldIdx = getSectionIndex(sect);
	QList<QModelIndex> oldChildIndices;

	if (oldCont  &&  !container) {
		buildModelIndexListRecurse(oldChildIndices, oldIdx);
	}

	sect->setID(id);
	sect->setVersion(version);
	sect->setContainerSection(container);

	QModelIndex newIdx = getSectionIndex(sect);

	changePersistentIndex(oldIdx, newIdx);

	if (oldCont  &&  !container) {
		for (QList<QModelIndex>::iterator it = oldChildIndices.begin() ; it != oldChildIndices.end() ; it++) {
			QModelIndex oldChildIdx = *it;

			changePersistentIndex(oldChildIdx, QModelIndex());
		}
	}

	emit sectionUpdated(sect, oldId, oldVer, oldCont);

	emit layoutChanged();
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

	if (parent) {
		parent->insertChild(parent->getChildBegin() + index, sect);
	} else {
		rootSects.insert(index, sect);
	}

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


void RWBSSectionModel::buildModelIndexListRecurse(QList<QModelIndex>& indices, QModelIndex idx)
{
	int rc = rowCount(idx);
	int cc = columnCount(idx);

	for (int i = 0 ; i < rc ; i++) {
		for (int j = 0 ; j < cc ; j++) {
			QModelIndex childIdx = index(i, j, idx);

			if (childIdx.isValid()) {
				indices << childIdx;
			}
		}

		QModelIndex childIdx = index(i, 0, idx);
		buildModelIndexListRecurse(indices, childIdx);
	}
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
			return QString(tr("<b>%1</b> [0x%2, %3 bytes @ 0x%4]"))
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
