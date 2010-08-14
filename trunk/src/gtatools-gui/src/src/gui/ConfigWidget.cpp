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

#include "ConfigWidget.h"
#include "../ProfileManager.h"
#include <qsettings.h>
#include <config.h>
#include "../System.h"



ConfigWidget::ConfigWidget(QWidget* parent)
		: QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui.setupUi(this);

	setWindowTitle(tr("ProgramBaseName") + windowTitle());

	ProfileManager* pm = ProfileManager::getInstance();
	ProfileManager::ProfileIterator it;

	for (it = pm->getProfileBegin() ; it != pm->getProfileEnd() ; it++) {
		Profile* profile = *it;
		ui.profileBox->addItem(profile->getName());
	}

	connect(ui.profileBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedProfileChanged(int)));
	connect(ui.applyButton, SIGNAL(clicked(bool)), this, SLOT(onApply(bool)));
	connect(ui.cancelButton, SIGNAL(clicked(bool)), this, SLOT(onCancel(bool)));

	selectedProfileChanged(0);

	QSettings settings(CONFIG_FILE, QSettings::IniFormat);

	QString regexSyntax = settings.value("main/regex_syntax", "wildcard").toString();

	if (regexSyntax == "wildcard") {
		ui.regexFormatBox->setCurrentIndex(0);
	} else if (regexSyntax == "full") {
		ui.regexFormatBox->setCurrentIndex(1);
	} else {
		ui.regexFormatBox->setCurrentIndex(0);
	}

	ui.fileTypesInTreeBox->setChecked(settings.value("gui/file_tree_types", true).toBool());
	ui.compactBox->setChecked(settings.value("gui/compact_mode", false).toBool());
}


void ConfigWidget::selectedProfileChanged(int index)
{

	Profile* profile = ProfileManager::getInstance()->getProfile(index);
	ui.profileWidget->displayProfile(profile);
}


void ConfigWidget::onApply(bool checked)
{
	QSettings settings(CONFIG_FILE, QSettings::IniFormat);

	switch (ui.regexFormatBox->currentIndex()) {
	case 0:
		settings.setValue("main/regex_syntax", "wildcard");
		break;
	case 1:
		settings.setValue("main/regex_syntax", "full");
		break;
	}

	settings.setValue("gui/file_tree_types", ui.fileTypesInTreeBox->isChecked());
	settings.setValue("gui/compact_mode", ui.compactBox->isChecked());

	Profile* profile = ProfileManager::getInstance()->getProfile(ui.profileBox->currentIndex());
	profile->setName(ui.profileWidget->getProfileName());
	profile->clearResources();

	QLinkedList<QString> resources;
	ui.profileWidget->getFiles(resources);
	QLinkedList<QString>::iterator it;

	for (it = resources.begin() ; it != resources.end() ; it++) {
		profile->addResource(File(it->toLocal8Bit().constData()));
	}

	ProfileManager::getInstance()->saveProfiles();
	profile->synchronize();

	settings.sync();

	System* sys = System::getInstance();
	sys->emitConfigurationChange();

	close();
}


void ConfigWidget::onCancel(bool checked)
{
	close();
}


