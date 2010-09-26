/*
 * FileTree.cpp
 *
 *  Created on: 14.08.2010
 *      Author: alemariusnexus
 */

#include "FileTree.h"
#include "../System.h"
#include "../ProfileManager.h"
#include "../formats/FormatHandler.h"
#include "../formats/FormatManager.h"
#include <QLinkedList>
#include <QStandardItemModel>



FileTree::FileTree(QWidget* parent)
		: QTreeView(parent), model(NULL)
{
	ProfileManager* pm = ProfileManager::getInstance();
	System* sys = System::getInstance();

	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this,
			SLOT(contextMenuRequested(const QPoint&)));
	connect(pm, SIGNAL(currentProfileChanged(Profile*, Profile*)), this,
			SLOT(currentProfileChanged(Profile*, Profile*)));
	connect(sys, SIGNAL(fileOpened(const FileOpenRequest&)), this,
			SLOT(fileOpened(const FileOpenRequest&)));
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
	header()->setResizeMode(0, QHeaderView::Stretch);
	header()->setResizeMode(1, QHeaderView::ResizeToContents);

	setSortingEnabled(true);
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
	File* file = static_cast<StaticFile*>(proxyModel->mapToSource(index).internalPointer())->getFile();

	if (!file->isDirectory()) {
		FileOpenRequest request(*file);
		sys->openFile(request);
	}
}


void FileTree::contextMenuRequested(const QPoint& pos)
{
	System* sys = System::getInstance();

	QModelIndex index = indexAt(pos);

	if (index.isValid()) {
		File* file = model->getFileForIndex(index);

		QMenu* menu = new QMenu(this);
		menu->setAttribute(Qt::WA_DeleteOnClose);

		QLinkedList<GUIModule*> modules = sys->getInstalledGUIModules();
		QLinkedList<GUIModule*>::iterator it;
		for (it = modules.begin() ; it != modules.end() ; it++) {
			GUIModule* module = *it;
			module->buildFileTreeMenu(*file, *menu);
		}

		if (!file->isDirectory()) {
			QLinkedList<FormatHandler*> handlers = FormatManager::getInstance()->getHandlers(*file);
			QLinkedList<FormatHandler*>::iterator it;
			for (it = handlers.begin() ; it != handlers.end() ; it++) {
				FormatHandler* handler = *it;
				handler->buildFileTreeMenu(*file, *menu);
			}
		}

		menu->popup(mapToGlobal(pos));
	}
}


void FileTree::currentProfileChanged(Profile* oldProfile, Profile* newProfile)
{
	if (oldProfile) {
		disconnect(oldProfile, SIGNAL(changed()), this, SLOT(currentProfileContentChanged()));
	}

	showProfile(newProfile);

	if (newProfile) {
		connect(newProfile, SIGNAL(changed()), this, SLOT(currentProfileContentChanged()));
	}
}


void FileTree::currentProfileContentChanged()
{
	reload();
}


void FileTree::fileOpened(const FileOpenRequest& request)
{
	QModelIndex cur = currentIndex();
	cur = proxyModel->mapToSource(cur);

	if (!cur.isValid()  ||  *model->getFileForIndex(cur) != *request.getFile()) {
		QModelIndex index = model->indexOf(*request.getFile(), rootIndex());

		if (index.isValid()) {
			setCurrentIndex(proxyModel->mapFromSource(index));
		}
	}
}

