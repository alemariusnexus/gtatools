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

#include <gtatools-gui/config.h>
#include "FileTree.h"
#include "../System.h"
#include "../ProfileManager.h"
#include "../formats/EntityHandler.h"
#include "../formats/EntityManager.h"
#include <QtCore/QLinkedList>
#include <QStandardItemModel>
#include <QtCore/QTime>
#include <QtCore/QSettings>




FileTree::FileTree(QWidget* parent)
		: QTreeView(parent), model(NULL)
{
	ProfileManager* pm = ProfileManager::getInstance();
	System* sys = System::getInstance();

	setContextMenuPolicy(Qt::CustomContextMenu);
	setSelectionMode(ExtendedSelection);

	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this,
			SLOT(contextMenuRequested(const QPoint&)));
	connect(pm, SIGNAL(currentProfileChanged(Profile*, Profile*)), this,
			SLOT(currentProfileChanged(Profile*, Profile*)));
	connect(sys, SIGNAL(entityOpened(DisplayedEntity*)), this,
			SLOT(entityOpened(DisplayedEntity*)));
	connect(sys, SIGNAL(configurationChanged()), this, SLOT(configurationChanged()));

	currentProfileChanged(NULL, pm->getCurrentProfile());
}


FileTree::~FileTree()
{
	if (model) {
		delete model;
		delete proxyModel;
	}
}


void FileTree::showProfile(Profile* profile)
{
	if (model) {
		delete model;
		delete proxyModel;
	}

	setSortingEnabled(false);

	proxyModel = new QSortFilterProxyModel;
	proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
	model = new FileItemModel(profile, this);
	proxyModel->setSourceModel(model);
	setModel(proxyModel);
	connect(this, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(fileSelected(const QModelIndex&)));

	header()->setStretchLastSection(false);

#if QT_VERSION >= 0x050000
	header()->setSectionResizeMode(0, QHeaderView::Stretch);
#else
	header()->setResizeMode(0, QHeaderView::Stretch);
#endif

	if (header()->count() > 1) {
#if QT_VERSION >= 0x050000
		header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
#else
		header()->setResizeMode(1, QHeaderView::ResizeToContents);
#endif
	}

	setSortingEnabled(true);
	sortByColumn(0, Qt::AscendingOrder);
}


void FileTree::reload()
{
	showProfile(ProfileManager::getInstance()->getCurrentProfile());
}


void FileTree::fileSelected(const QModelIndex& index)
{
	if (!index.isValid()  ||  index == rootIndex()) {
		return;
	}

	System* sys = System::getInstance();
	File file = static_cast<StaticFile*>(proxyModel->mapToSource(index).internalPointer())->getFile();

	if (!file.isDirectory()) {
		sys->openFile(file);
	}
}


void FileTree::contextMenuRequested(const QPoint& pos)
{
	System* sys = System::getInstance();

	QLinkedList<File> selectedFiles;
	QModelIndexList sel = selectionModel()->selectedIndexes();
	QModelIndexList::iterator iit;

	for (iit = sel.begin() ; iit != sel.end() ; iit++) {
		if (iit->column() != 0) {
			continue;
		}

		QModelIndex idx = proxyModel->mapToSource(*iit);
		File file = model->getFileForIndex(idx);
		selectedFiles << file;
	}

	QMenu* menu = new QMenu(this);
	menu->setAttribute(Qt::WA_DeleteOnClose);

	QLinkedList<GUIModule*> modules = sys->getInstalledGUIModules();
	QLinkedList<GUIModule*>::iterator it;
	for (it = modules.begin() ; it != modules.end() ; it++) {
		GUIModule* module = *it;
		module->buildFileTreeMenu(selectedFiles, *menu);
	}

	menu->popup(mapToGlobal(pos));
}


void FileTree::currentProfileChanged(Profile* oldProfile, Profile* newProfile)
{
	if (oldProfile) {
		disconnect(oldProfile, SIGNAL(searchResourceAdded(const File&)), this,
				SLOT(currentProfileResourceAdded(const File&)));
		disconnect(oldProfile, SIGNAL(searchResourceRemoved(const File&)), this,
				SLOT(currentProfileResourceRemoved(const File&)));
	}

	showProfile(newProfile);

	if (newProfile) {
		connect(newProfile, SIGNAL(searchResourceAdded(const File&)), this,
				SLOT(currentProfileResourceAdded(const File&)));
		connect(newProfile, SIGNAL(searchResourceRemoved(const File&)), this,
				SLOT(currentProfileResourceRemoved(const File&)));
	}
}


void FileTree::entityOpened(DisplayedEntity* ent)
{
	DisplayedFile* file = dynamic_cast<DisplayedFile*>(ent);

	if (file) {
		QSettings settings;

		if (settings.value("gui/file_tree_auto_select", true).toBool()) {
			QModelIndex cur = currentIndex();
			cur = proxyModel->mapToSource(cur);

			if (!cur.isValid()  ||  model->getFileForIndex(cur) != file->getFile()) {
				QModelIndex index = model->indexOf(file->getFile(), rootIndex());

				if (index.isValid()) {
					QModelIndex idx = proxyModel->mapFromSource(index);
					setCurrentIndex(idx);
				}
			}
		}
	}
}


void FileTree::configurationChanged()
{
	model->reload();
	//QAbstractItemModel* m = QTreeView::model();
	//setModel(NULL);
	//setModel(m);
}

