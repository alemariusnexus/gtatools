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

#include "DFFTextureSourceDialog.h"
#include "DFFTextureSourceSelectDialog.h"



DFFTextureSourceDialog::DFFTextureSourceDialog(QWidget* parent)
		: QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.addButton, SIGNAL(pressed()), this, SLOT(addRequested()));
	connect(ui.editButton, SIGNAL(pressed()), this, SLOT(editRequested()));
	connect(ui.removeButton, SIGNAL(pressed()), this, SLOT(removeRequested()));
	connect(ui.list, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this,
			SLOT(currentSourceChanged(QListWidgetItem*, QListWidgetItem*)));
}


DFFTextureSourceDialog::~DFFTextureSourceDialog()
{
	while (ui.list->count() != 0)
		delete ui.list->takeItem(0);
}


void DFFTextureSourceDialog::currentSourceChanged(QListWidgetItem* item, QListWidgetItem* prev)
{
	ui.removeButton->setEnabled(item != NULL);
	ui.editButton->setEnabled(item != NULL);
}


void DFFTextureSourceDialog::addTextureSource(QString source, bool path)
{
	QListWidgetItem* item = new QListWidgetItem(source);
	item->setData(Qt::UserRole, path);
	ui.list->addItem(item);
}


QString DFFTextureSourceDialog::getTextureSource(int idx, bool& path) const
{
	QListWidgetItem* item = ui.list->item(idx);
	path = item->data(Qt::UserRole).toBool();
	return item->text();
}


void DFFTextureSourceDialog::addRequested()
{
	DFFTextureSourceSelectDialog dlg(this);

	if (dlg.exec() == QDialog::Accepted) {
		addTextureSource(dlg.getSource(), dlg.isPathSource());
	}
}


void DFFTextureSourceDialog::editRequested()
{
	QListWidgetItem* item = ui.list->currentItem();
	bool path = item->data(Qt::UserRole).toBool();

	DFFTextureSourceSelectDialog dlg(this);
	dlg.setSource(item->text(), path);

	if (dlg.exec() == QDialog::Accepted) {
		item->setText(dlg.getSource());
		item->setData(Qt::UserRole, dlg.isPathSource());
	}
}


void DFFTextureSourceDialog::removeRequested()
{
	QListWidgetItem* item = ui.list->currentItem();
	ui.list->takeItem(ui.list->row(item));
	delete item;
}
