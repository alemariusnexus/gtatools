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

#include "PVSDialog.h"
#include "../System.h"
#include <gta/Engine.h>
#include <gta/scene/visibility/PVSDatabase.h>
#include <QFileDialog>
#include <QThread>



PVSDialog::PVSDialog(QWidget* parent)
		: QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));

	int tc = QThread::idealThreadCount();

	ui.threadCountSpinner->setValue(tc == -1 ? 1 : tc);

#ifdef GTAFORMATS_LITTLE_ENDIAN
	ui.littleEndianBox->setChecked(true);
#else
	ui.littleEndianBox->setChecked(false);
#endif
}


void PVSDialog::progressChanged(int value, int max)
{
	int percentage = (int) (((float) value / (float) max) * 100.0f);
	task->update(percentage);
	qApp->processEvents();
}


void PVSDialog::buttonClicked(QAbstractButton* button)
{
	if (ui.buttonBox->standardButton(button) == QDialogButtonBox::Ok) {
		Engine* engine = Engine::getInstance();
		Scene* scene = engine->getScene();

		float sx = (float) ui.xSizeBox->value();
		float sy = (float) ui.ySizeBox->value();
		float sz = (float) ui.zSizeBox->value();

		PVSDatabase* pvs = scene->getStreamingManager()->getPVSDatabase();
		pvs->addProgressObserver(this);

		task = System::getInstance()->createTask();
		task->start(0, 100, tr("Building PVS data..."));

		pvs->calculateSections(sx, sy, sz);
		pvs->calculatePVS(ui.threadCountSpinner->value());

		delete task;

		QString fname = QFileDialog::getSaveFileName(this, tr("Select destination file"), QString(),
				tr("PVS files (*.pvs)"));

		if (!fname.isNull()) {
			File file(fname.toLocal8Bit().constData());

			bool littleEndian = ui.littleEndianBox->isChecked();

			pvs->save(file,
					littleEndian ? PVSDatabase::SaveForceLittleEndian : PVSDatabase::SaveForceBigEndian);

			accept();
		}

		accept();
	} else {
		reject();
	}
}
