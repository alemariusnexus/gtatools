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

#include "RWBSSectionTree.h"
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include "RWBSSectionDialog.h"



RWBSSectionTree::RWBSSectionTree(RWBSSectionModel* model, QWidget* parent)
		: QTreeView(parent), model(model), undoStack(NULL)
{
	setModel(model);
	setItemDelegate(&delegate);

	addAction = new QAction(tr("Add Section"), this);
	removeAction = new QAction(tr("Remove Section"), this);
	editAction = new QAction(tr("Edit Section"), this);

	QTreeView::addAction(addAction);
	QTreeView::addAction(removeAction);
	QTreeView::addAction(editAction);

	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this,
			SLOT(contextMenuRequested(const QPoint&)));
	connect(addAction, SIGNAL(triggered(bool)), this, SLOT(addRequested(bool)));
	connect(removeAction, SIGNAL(triggered(bool)), this, SLOT(removeRequested(bool)));
	connect(editAction, SIGNAL(triggered(bool)), this, SLOT(editRequested(bool)));
	connect(model, SIGNAL(sectionRemoved(RWSection*, RWSection*)), this,
			SIGNAL(sectionRemoved(RWSection*, RWSection*)));
	connect(model, SIGNAL(sectionInserted(RWSection*)), this,
			SIGNAL(sectionInserted(RWSection*)));
	connect(model, SIGNAL(sectionUpdated(RWSection*, uint32_t, uint32_t, bool)), this,
			SIGNAL(sectionUpdated(RWSection*, uint32_t, uint32_t, bool)));

	setContextMenuPolicy(Qt::CustomContextMenu);
}


void RWBSSectionTree::setEditable(bool editable)
{
	this->editable = editable;

	addAction->setEnabled(editable);
	removeAction->setEnabled(editable);
	editAction->setEnabled(editable);
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
	if (!editable)
		return;

	RWBSSectionDialog dialog(this);

	if (contextSect)
		dialog.setVersion(contextSect->getVersion());

	if (dialog.exec() == QDialog::Accepted) {
		int32_t id = dialog.getID();
		bool container = dialog.isContainerSection();

		RWSection* sect = new RWSection(id, dialog.getVersion());

		if (!container)
			sect->setData(new uint8_t[0], 0);

		QUndoCommand* cmd = model->createInsertSectionCommand(sect, contextSect);

		if (undoStack) {
			undoStack->push(cmd);
		} else {
			cmd->redo();
			delete cmd;
		}
	}
}


void RWBSSectionTree::removeRequested(bool)
{
	if (!editable)
		return;

	QUndoCommand* cmd = model->createRemoveSectionCommand(contextSect);

	if (undoStack) {
		undoStack->push(cmd);
	} else {
		cmd->redo();
		delete cmd;
	}
}


void RWBSSectionTree::editRequested(bool)
{
	if (!editable)
		return;

	RWBSSectionDialog dialog(this);
	dialog.setID(contextSect->getID());
	dialog.setContainerSection(!contextSect->isDataSection());
	dialog.setVersion(contextSect->getVersion());

	if (dialog.exec() == QDialog::Accepted) {
		int32_t id = dialog.getID();
		bool container = dialog.isContainerSection();
		int32_t version = dialog.getVersion();

		RWSection* oldParent = contextSect->getParent();

		if (contextSect->isDataSection()  &&  container  &&  !contextSect->canBeConvertedToContainerSection()) {
			QMessageBox::critical(this, tr("Invalid Conversion"),
					tr("This section can't be converted to a valid container section!"));
			return;
		}

		QUndoCommand* cmd = model->createUpdateSectionCommand(contextSect, id, version, container);

		if (undoStack) {
			undoStack->push(cmd);
		} else {
			cmd->redo();
			delete cmd;
		}

		//contextSect->setID(id);
		//contextSect->setVersion(version);

		//emit sectionChanged(contextSect);

		/*if (container != !contextSect->isDataSection()) {
			int index = model.removeSection(contextSect);
			contextSect->setDataSection(!container);
			model.insertSection(contextSect, oldParent, index);
		} else {
			model.applySectionChanges(contextSect);
		}*/
	}
}
