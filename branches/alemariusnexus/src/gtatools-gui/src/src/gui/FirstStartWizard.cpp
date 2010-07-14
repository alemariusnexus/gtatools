/*
 * FirstStartWizard.cpp
 *
 *  Created on: 11.07.2010
 *      Author: alemariusnexus
 */

#include "FirstStartWizard.h"
#include <config.h>
#include <qsettings.h>
#include <cstdio>
#include <qfiledialog.h>
#include <gtaformats/util/File.h>
#include <qlinkedlist.h>



FirstStartWizard::FirstStartWizard()
{
	ui.setupUi(this);
	setWindowTitle(tr("ProgramBaseName") + windowTitle());

	connect(this, SIGNAL(finished(int)), this, SLOT(finished(int)));
}


void FirstStartWizard::finished(int result)
{
	if (result == Accepted) {
		QSettings settings(CONFIG_FILE, QSettings::IniFormat);

		settings.setValue("main/current_profile", 0);
		settings.setValue("profile0/name", ui.profileWidget->getProfileName());

		QLinkedList<QString> files;
		ui.profileWidget->getFiles(files);
		QLinkedList<QString>::iterator it;

		int i = 0;

		for (it = files.begin() ; it != files.end() ; it++, i++) {
			settings.setValue(QString("profile0/resource%1").arg(i), *it);
		}

		settings.sync();
	}
}

