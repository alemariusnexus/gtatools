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

#include "RWBSSectionDialog.h"
#include <gtaformats/rwbs/RWSection.h>
#include <gtaformats/gta.h>
#include <QString>
#include "../../UInt64Validator.h"



RWBSSectionDialog::RWBSSectionDialog(QWidget* parent)
		: QDialog(parent)
{
	ui.setupUi(this);

	ui.idField->setValidator(new UInt64Validator(0, UINT32_MAX, this));
	ui.versionField->setValidator(new UInt64Validator(0, UINT32_MAX, this));

	for (unsigned int i = 0 ; i < RW_NUM_KNOWN_SECTIONS ; i++) {
		RWSectionInfo info = RWKnownSections[i];
		ui.idBox->addItem(info.name, info.id);
	}

	ui.versionBox->addItem(tr("GTA 3 [1]"), RW_VERSION_GTA3_1);
	ui.versionBox->addItem(tr("GTA 3 [2]"), RW_VERSION_GTA3_2);
	ui.versionBox->addItem(tr("GTA Vice City [1]"), RW_VERSION_GTAVC_1);
	ui.versionBox->addItem(tr("GTA Vice City [2]"), RW_VERSION_GTAVC_2);
	ui.versionBox->addItem(tr("GTA San Andreas"), RW_VERSION_GTASA);

	connect(ui.idBox, SIGNAL(activated(int)), this, SLOT(idBoxChanged(int)));
	connect(ui.versionBox, SIGNAL(activated(int)), this, SLOT(versionBoxChanged(int)));
}


int32_t RWBSSectionDialog::getID() const
{
	int32_t id = (int32_t) ui.idField->text().toUInt(NULL, 0);
	return id;
}


int32_t RWBSSectionDialog::getVersion() const
{
	return (int32_t) ui.versionField->text().toUInt(NULL, 0);
}


void RWBSSectionDialog::idBoxChanged(int idx)
{
	if (idx == 0) {
		ui.idField->setEnabled(true);
		return;
	} else {
		ui.idField->setEnabled(false);
	}

	int32_t id = ui.idBox->itemData(idx).toInt(NULL);
	ui.idField->setText("0x" + QString("%1").arg(id, 0, 16).toUpper());

	RWSectionInfo info;

	if (RWGetSectionInfo(id, &info)) {
		ui.containerBox->setChecked(info.container);
	}
}


void RWBSSectionDialog::versionBoxChanged(int idx)
{
	if (idx == 0) {
		ui.versionField->setEnabled(true);
		return;
	} else {
		ui.versionField->setEnabled(false);
	}

	int32_t version = ui.versionBox->itemData(idx).toInt(NULL);
	ui.versionField->setText("0x" + QString("%1").arg(version, 0, 16).toUpper());
}


void RWBSSectionDialog::setID(int32_t id)
{
	ui.idField->setText("0x" + QString("%1").arg(id, 0, 16).toUpper());

	int idx = ui.idBox->findData(id);

	if (idx == -1)
		idx = 0;

	ui.idBox->setCurrentIndex(idx);
	ui.idField->setEnabled(idx == 0);
}


void RWBSSectionDialog::setVersion(int32_t version)
{
	ui.versionField->setText("0x" + QString("%1").arg(version, 0, 16).toUpper());

	int idx = ui.versionBox->findData(version);

	if (idx == -1)
		idx = 0;

	ui.versionBox->setCurrentIndex(idx);
	ui.versionField->setEnabled(idx == 0);
}
