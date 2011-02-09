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
 */

#include "DFFFrameItemModel.h"
#include <cstdio>



Qt::ItemFlags DFFFrameItemModel::flags(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return 0;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant DFFFrameItemModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}

	DFFFrame* frame = (DFFFrame*) index.internalPointer();

	if (role == Qt::DisplayRole) {
		if (frame->getName() == NULL) {
			//int row = frame->getParent() == NULL ? frame->getParent()->indexOf(frame) : mesh->indexOf(frame);
			int row = frame->getParent()->indexOf(frame);
			return tr("Unnamed %1").arg(row+1);
		} else {
			return frame->getName();
		}
	} else {
		return QVariant();
	}
}


QVariant DFFFrameItemModel::headerData(int section, Qt::Orientation orientation, int role) const
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


int DFFFrameItemModel::rowCount(const QModelIndex& index) const
{
	if (index.column() > 0) {
		return 0;
	}

	if (!index.isValid()) {
		return mesh->getRootFrame()->getChildCount();
	} else {
		DFFFrame* frame = (DFFFrame*) index.internalPointer();
		return frame->getChildCount();
	}
}


int DFFFrameItemModel::columnCount(const QModelIndex& index) const
{
	return 1;
}


QModelIndex DFFFrameItemModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	DFFFrame* parentFrame;

	if (parent.isValid()) {
		parentFrame = (DFFFrame*) parent.internalPointer();
	} else {
		parentFrame = mesh->getRootFrame();
	}

	return createIndex(row, column, parentFrame->getChild(row));
}


QModelIndex DFFFrameItemModel::parent(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return QModelIndex();
	}

	DFFFrame* frame = (DFFFrame*) index.internalPointer();
	DFFFrame* parent = frame->getParent();

	if (parent == NULL  ||  parent->isRoot()) {
		return QModelIndex();
	}

	int row = parent->getParent()->indexOf(parent);
	return createIndex(row, 0, parent);
}

