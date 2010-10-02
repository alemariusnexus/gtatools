/*
 * DFFFrameItemModel.cpp
 *
 *  Created on: 06.08.2010
 *      Author: alemariusnexus
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

