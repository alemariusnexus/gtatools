/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#ifndef FILETREE_H_
#define FILETREE_H_

#include <QTreeView>
#include <QWidget>
#include <QtCore/QModelIndex>
#include <QtCore/QPoint>
#include "FileItemModel.h"
#include <QtCore/QHash>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include "../EntityOpenRequest.h"
#include <QSortFilterProxyModel>
#include "../DisplayedFile.h"



class FileTree : public QTreeView {
	Q_OBJECT

public:
	FileTree(QWidget* parent = NULL);
	~FileTree();

private:
	void showProfile(Profile* profile);
	void reload();

private slots:
	void fileSelected(const QModelIndex& index);
	void contextMenuRequested(const QPoint& pos);
	void currentProfileChanged(Profile* oldProfile, Profile* newProfile);
	void currentProfileResourceAdded(const File& file) { reload(); }
	void currentProfileResourceRemoved(const File& file) { reload(); }
	void entityOpened(DisplayedEntity* ent);
	void configurationChanged();

private:
	QSortFilterProxyModel* proxyModel;
	FileItemModel* model;
};

#endif /* FILETREE_H_ */
