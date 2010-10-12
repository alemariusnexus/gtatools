/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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
 */

#include "FirstStartWizard.h"
#include "../config.h"
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

