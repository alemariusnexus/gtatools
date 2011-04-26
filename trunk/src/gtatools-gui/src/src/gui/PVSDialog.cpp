/*
 * PVSDialog.cpp
 *
 *  Created on: 21.03.2011
 *      Author: alemariusnexus
 */

#include "PVSDialog.h"
#include "../Profile.h"
#include "../ProfileManager.h"
#include <gta/Engine.h>
#include <gta/scene/visibility/PVSData.h>
#include <gta/scene/Scene.h>
#include <QtGui/QFileDialog>
#include "../System.h"



PVSDialog::PVSDialog(QWidget* parent)
		: QDialog(parent)
{
	ui.setupUi(this);

	connect(ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this,
			SLOT(buttonClicked(QAbstractButton*)));
}


void PVSDialog::buttonClicked(QAbstractButton* button)
{
	if (ui.buttonBox->standardButton(button) == QDialogButtonBox::Ok) {
		accept();

		Profile* profile = ProfileManager::getInstance()->getCurrentProfile();

		Engine* engine = Engine::getInstance();

		Scene* scene = new Scene;
		engine->setScene(scene);

		File datRoot(profile->getDATRootDirectory().toLocal8Bit().constData());

		Profile::ResourceIterator it;

		for (it = profile->getDATFilesBegin() ; it != profile->getDATFilesEnd() ; it++) {
			engine->loadDAT(**it, datRoot);
		}

		//engine->loadDAT(File(*file, "data/default.dat"), *file);
		//engine->loadDAT(File(*file, "data/gta.dat"), *file);

		PVSData pvs(scene);
		pvs.addProgressObserver(this);
		pvs.setSectionSize((float) ui.sectSizeXSpinner->value(), (float) ui.sectSizeYSpinner->value(),
				(float) ui.sectSizeZSpinner->value());

		task = System::getInstance()->createTask();
		task->start(0, 100, tr("Generating PVS Data..."));
		pvs.build();
		delete task;

		QString fname = QFileDialog::getSaveFileName(this, tr("Choose a file to store PVS data"), QString(),
				tr("PVS files (*.pvs)"));

		if (fname.isNull()) {
			delete scene;
			return;
		}

		pvs.serialize(File(fname.toLocal8Bit().constData()));

		delete scene;
	} else {
		reject();
	}
}


void PVSDialog::progressChanged(int value, int max)
{
	task->update(value);
	qApp->processEvents();
}