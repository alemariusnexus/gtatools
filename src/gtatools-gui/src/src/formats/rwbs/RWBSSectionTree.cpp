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

#include "RWBSSectionTree.h"
#include <QtGui/QMenu>
#include "RWBSSectionDialog.h"



RWBSSectionTree::RWBSSectionTree(QWidget* parent)
		: QTreeView(parent)
{
	setModel(&model);

	addAction = new QAction(tr("Add Section"), this);
	removeAction = new QAction(tr("Remove Section"), this);
	editAction = new QAction(tr("Edit Section"), this);

	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this,
			SLOT(contextMenuRequested(const QPoint&)));
	connect(addAction, SIGNAL(triggered(bool)), this, SLOT(addRequested(bool)));
	connect(removeAction, SIGNAL(triggered(bool)), this, SLOT(removeRequested(bool)));
	connect(editAction, SIGNAL(triggered(bool)), this, SLOT(editRequested(bool)));
	connect(&model, SIGNAL(sectionRemoved(RWSection*, RWSection*)), this,
			SIGNAL(sectionRemoved(RWSection*, RWSection*)));
	connect(&model, SIGNAL(sectionInserted(RWSection*)), this,
			SIGNAL(sectionInserted(RWSection*)));

	setContextMenuPolicy(Qt::CustomContextMenu);
}


void RWBSSectionTree::contextMenuRequested(const QPoint& pos)
{
	QModelIndex index = indexAt(pos);

	RWSection* sect = NULL;

	if (index.isValid())
		sect = (RWSection*) index.internalPointer();

	contextSect = sect;

	QMenu* menu = new QMenu(this);
	menu->setAttribute(Qt::WA_DeleteOnClose);

	if (!sect  ||  !sect->isDataSection())
		menu->addAction(addAction);

	if (sect) {
		menu->addAction(removeAction);
		menu->addAction(editAction);
	}

	menu->popup(mapToGlobal(pos));
}


void RWBSSectionTree::addRequested(bool)
{
	RWBSSectionDialog dialog(this);

	if (contextSect)
		dialog.setVersion(contextSect->getVersion());

	if (dialog.exec() == QDialog::Accepted) {
		int32_t id = dialog.getID();
		bool container = dialog.isContainerSection();

		RWSection* sect = new RWSection(id, dialog.getVersion());
		model.insertSection(sect, contextSect);

		if (!container)
			sect->setData(new uint8_t[0], 0);
	}
}


void RWBSSectionTree::removeRequested(bool)
{
	RWSection* parent = contextSect->getParent();

	model.removeSection(contextSect);

	if (parent)
		delete contextSect;
}


void RWBSSectionTree::editRequested(bool)
{
	RWBSSectionDialog dialog(this);
	dialog.setID(contextSect->getID());
	dialog.setContainerSection(!contextSect->isDataSection());
	dialog.setVersion(contextSect->getVersion());

	if (dialog.exec() == QDialog::Accepted) {
		int32_t id = dialog.getID();
		bool container = dialog.isContainerSection();
		int32_t version = dialog.getVersion();

		RWSection* oldParent = contextSect->getParent();

		contextSect->setID(id);
		contextSect->setVersion(version);

		emit sectionChanged(contextSect);

		if (container != !contextSect->isDataSection()) {
			int index = model.removeSection(contextSect);
			contextSect->setDataSection(!container);
			model.insertSection(contextSect, oldParent, index);
		} else {
			model.applySectionChanges(contextSect);
		}
	}
}
