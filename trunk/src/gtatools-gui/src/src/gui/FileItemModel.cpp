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

#include "FileItemModel.h"
#include "../formats/FormatManager.h"
#include <cstring>
#include <qprogressdialog.h>
#include <qsettings.h>
#include <config.h>



FileItemModel::FileItemModel(Profile* profile, QWidget* parent)
		: QAbstractItemModel(parent), profile(profile)
{
	QSettings settings(CONFIG_FILE, QSettings::IniFormat);

	showFileType = settings.value("gui/file_tree_types", true).toBool();

	rootFile = new StaticFile;
	Profile::ResourceIterator it;

	for (it = profile->getResourceBegin() ; it != profile->getResourceEnd() ; it++) {
		rootFile->addRootChild(new StaticFile(**it));
	}
}


QModelIndex FileItemModel::indexOf(const File& file, const QModelIndex& start)
{
	int rc = rowCount(start);

	for (int i = 0 ; i < rc ; i++) {
		//QModelIndex child = start.child(i, 0);
		QModelIndex child = index(i, 0, start);

		StaticFile* sf = static_cast<StaticFile*>(child.internalPointer());

		if (*sf->getFile() == file) {
			return child;
		} else if (file.isChildOf(*sf->getFile())) {
			return indexOf(file, child);
		}
	}

	return QModelIndex();
}


File* FileItemModel::getFileForIndex(const QModelIndex& index)
{
	return static_cast<StaticFile*>(index.internalPointer())->getFile();
}


int FileItemModel::rowCount(const QModelIndex& parent) const
{
	if (parent.column() > 0) {
		return 0;
	}

	StaticFile* parentFile;

	if (parent.isValid()) {
		parentFile = static_cast<StaticFile*>(parent.internalPointer());
	} else {
		parentFile = rootFile;
	}

	return parentFile->getChildCount();
}


Qt::ItemFlags FileItemModel::flags(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return 0;
	}

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant FileItemModel::headerData(int section, Qt::Orientation orient, int role) const
{
	if (orient != Qt::Horizontal) {
		return QVariant();
	}

	if (role == Qt::DisplayRole) {
		switch (section) {
		case 0:
			return tr("Filename");
			break;
		case 1:
			return tr("File Type");
			break;
		}
	}

	return QVariant();
}


QVariant FileItemModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}
	if (role != Qt::DisplayRole) {
		return QVariant();
	}

	StaticFile* file = static_cast<StaticFile*>(index.internalPointer());

	switch (index.column()) {
	case 0:
		return QVariant(file->toString());
		break;
	case 1:
		File* realFile = file->getFile();

		if (realFile->isDirectory()) {
			return tr("Directory");
		} else {
			FormatHandler* handler = FormatManager::getInstance()->getHandler(*realFile);

			if (handler) {
				return handler->getFormatName(realFile);
			} else {
				return tr("Unrecognized File");
			}
		}
	}
}


QModelIndex FileItemModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	StaticFile* parentFile;

	if (parent.isValid()) {
		parentFile = static_cast<StaticFile*>(parent.internalPointer());
	} else {
		parentFile = rootFile;
	}

	return createIndex(row, column, parentFile->getChild(row));
}


QModelIndex FileItemModel::parent(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return QModelIndex();
	}

	StaticFile* file = static_cast<StaticFile*>(index.internalPointer());
	StaticFile* parent = file->getParent();

	if (parent == rootFile) {
		return QModelIndex();
	}

	return createIndex(parent->getParent()->indexOf(parent), 0, parent);
}

