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

#include "IFPFrameItemModel.h"
#include <cstdio>



/*QModelIndex DFFFrameItemModel::getFrameIndex(const DFFFrame* frame) const
{
	const DFFFrame* parent = frame->getParent();

	if (parent) {
		QModelIndex parentIdx = getFrameIndex(parent);
		int row = parent->indexOf(frame);
		QModelIndex idx = index(row, 0, parentIdx);
		return idx;
	} else {
		return QModelIndex();
	}
}*/


Qt::ItemFlags IFPFrameItemModel::flags(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return 0;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant IFPFrameItemModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}

	MeshFrame* frame = (MeshFrame*) index.internalPointer();

	if (role == Qt::DisplayRole) {
		if (frame->getName().get() == NULL) {
			//int row = frame->getParent() == NULL ? frame->getParent()->indexOf(frame) : mesh->indexOf(frame);

			int row = frame->getParent()->indexOf(frame);
			return tr("Unnamed %1").arg(row+1);
		} else {
			return QString(frame->getName().get());
		}
	} else {
		return QVariant();
	}
}


QVariant IFPFrameItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (section != 0  ||  orientation != Qt::Horizontal) {
		return QVariant();
	}

	if (role == Qt::DisplayRole) {
		return tr("Frame");
	} else {
		return QVariant();
	}
}


int IFPFrameItemModel::rowCount(const QModelIndex& index) const
{
	if (index.column() > 0) {
		return 0;
	}

	if (!index.isValid()) {
		if (clump)
			return clump->getRootFrame()->getChildCount();
		else
			return 0;
	} else {
		MeshFrame* frame = (MeshFrame*) index.internalPointer();
		return frame->getChildCount();
	}
}


int IFPFrameItemModel::columnCount(const QModelIndex& index) const
{
	return 1;
}


QModelIndex IFPFrameItemModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	MeshFrame* parentFrame;

	if (parent.isValid()) {
		parentFrame = (MeshFrame*) parent.internalPointer();
	} else {
		parentFrame = clump->getRootFrame();
	}

	return createIndex(row, column, parentFrame->getChild(row));
}


QModelIndex IFPFrameItemModel::parent(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return QModelIndex();
	}

	MeshFrame* frame = (MeshFrame*) index.internalPointer();
	MeshFrame* parent = frame->getParent();

	if (parent == NULL  ||  parent->getParent() == NULL) {
		return QModelIndex();
	}

	int row = parent->getParent()->indexOf(parent);
	return createIndex(row, 0, parent);
}

