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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "DFFTextureSourceSelectDialog.h"
#include <QtGui/QFileDialog>



DFFTextureSourceSelectDialog::DFFTextureSourceSelectDialog(QWidget* parent)
		: QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.nameButton, SIGNAL(toggled(bool)), ui.nameField, SLOT(setEnabled(bool)));

	connect(ui.pathButton, SIGNAL(toggled(bool)), ui.pathField, SLOT(setEnabled(bool)));
	connect(ui.pathButton, SIGNAL(toggled(bool)), ui.pathChooseButton, SLOT(setEnabled(bool)));

	connect(ui.pathChooseButton, SIGNAL(pressed()), this, SLOT(pathChosen()));

	ui.nameButton->setChecked(true);
}


void DFFTextureSourceSelectDialog::setSource(const QString& source, bool path)
{
	if (path) {
		ui.pathButton->setChecked(true);
		ui.pathField->setText(source);
	} else {
		ui.nameButton->setChecked(true);
		ui.nameField->setText(source);
	}
}


void DFFTextureSourceSelectDialog::pathChosen()
{
	QString path = QFileDialog::getOpenFileName(this, tr("Select Texture Source"), QString(),
			tr("Texture Dictionaries (*.txd)"));

	if (!path.isNull()) {
		ui.pathField->setText(path);
	}
}
