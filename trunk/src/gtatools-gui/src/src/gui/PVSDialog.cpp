/*
 * PVSDialog.cpp
 *
 *  Created on: 20.10.2011
 *      Author: alemariusnexus
 */

#include "PVSDialog.h"
#include "../System.h"
#include <gta/Engine.h>
#include <gta/scene/visibility/PVSDatabase.h>
#include <QtGui/QFileDialog>
#include <QtCore/QThread>



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

		PVSDatabase pvs;
		pvs.addProgressObserver(this);
		pvs.setSectionSize(sx, sy, sz);

		task = System::getInstance()->createTask();
		task->start(0, 100, tr("Building PVS data..."));

		pvs.buildSections(scene->getSceneObjectBegin(), scene->getSceneObjectEnd());
		pvs.calculatePVS(scene->getSceneObjectBegin(), scene->getSceneObjectEnd(),
				ui.threadCountSpinner->value());

		delete task;

		QString fname = QFileDialog::getSaveFileName(this, tr("Select destination file"), QString(),
				tr("PVS files (*.pvs)"));

		if (!fname.isNull()) {
			File file(fname.toLocal8Bit().constData());

			bool littleEndian = ui.littleEndianBox->isChecked();

			pvs.save(file, scene->getDefinitionDatabase(),
					littleEndian ? PVSDatabase::SaveForceLittleEndian : PVSDatabase::SaveForceBigEndian);

			accept();
		}

		/*PVSData pvs(scene);
		pvs.setSectionSize(sx, sy, sz);
		pvs.addProgressObserver(this);

		task = System::getInstance()->createTask();
		task->start(0, 100, tr("Building PVS data..."));

		pvs.build();

		delete task;

		QString fname = QFileDialog::getSaveFileName(this, tr("Select destination file"), QString(),
				tr("PVS files (*.pvs)"));

		if (!fname.isNull()) {
			File file(fname.toLocal8Bit().constData());
			pvs.serialize(file);

			accept();
		}*/
		accept();
	} else {
		reject();
	}
}
