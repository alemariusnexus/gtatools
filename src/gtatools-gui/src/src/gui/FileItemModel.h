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

#ifndef FILEITEMMODEL_H_
#define FILEITEMMODEL_H_

#include <QtCore/QAbstractItemModel>
#include <QtCore/QMap>
#include <QtCore/QList>
#include "../Profile.h"
#include <gtaformats/util/File.h>
#include "StaticFile.h"
#include <QtGui/QWidget>


class FileItemModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	FileItemModel(Profile* profile, QWidget* parent = NULL);
	virtual ~FileItemModel();

	QModelIndex indexOf(const File& file, const QModelIndex& start);
	File* getFileForIndex(const QModelIndex& index);

	int rowCount(const QModelIndex& parent = QModelIndex()) const;
	int columnCount(const QModelIndex& parent = QModelIndex()) const { return showFileType ? 2 : 1; }
	Qt::ItemFlags flags(const QModelIndex& index) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orient, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex& parent) const;

private:
	Profile* profile;
	QList<StaticFile*> rootFiles;
	bool showFileType;
};

#endif /* FILEITEMMODEL_H_ */
