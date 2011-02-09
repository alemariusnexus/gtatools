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

#ifndef DFFFRAMEITEMMODEL_H_
#define DFFFRAMEITEMMODEL_H_

#include <gtaformats/dff/DFFFrame.h>
#include <gtaformats/dff/DFFMesh.h>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QVariant>


class DFFFrameItemModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	DFFFrameItemModel(DFFMesh* mesh) : mesh(mesh) {}
	Qt::ItemFlags flags(const QModelIndex& index) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	int rowCount(const QModelIndex& index = QModelIndex()) const;
	int columnCount(const QModelIndex& index) const;
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& index) const;

private:
	DFFMesh* mesh;
};

#endif /* DFFFRAMEITEMMODEL_H_ */
