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

#include <gtatools-gui/config.h>
#include "ConfigWidget.h"
#include "ProfileConfigWidget.h"
#include "../ProfileManager.h"
#include <QtCore/QSettings>
#include "../System.h"
#include <QtCore/QVariant>



ConfigWidget::ConfigWidget(QWidget* parent)
		: QWidget(parent), profiles(ProfileManager::getInstance()->getProfiles())
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui.setupUi(this);

	setWindowTitle(tr("ProgramBaseName") + windowTitle());

	ProfileManager* pm = ProfileManager::getInstance();
	ProfileManager::ProfileIterator it;

	for (it = pm->getProfileBegin() ; it != pm->getProfileEnd() ; it++) {
		Profile* profile = *it;
		ui.profileBox->addItem(profile->getName(), QVariant::fromValue((void*) profile));
		ui.profileStackedWidget->addWidget(new ProfileConfigWidget(profile, ui.profileStackedWidget));
	}

	connect(ui.profileBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectedProfileChanged(int)));
	connect(ui.newProfileButton, SIGNAL(clicked(bool)), this, SLOT(onNewProfile(bool)));
	connect(ui.removeProfileButton, SIGNAL(clicked(bool)), this, SLOT(onRemoveProfile(bool)));
	connect(ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));

	QSettings settings;

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
	ui.fileTreeAutoSelectBox->setChecked(settings.value("gui/file_tree_auto_select", true).toBool());

	selectedProfileChanged(profiles.indexOf(pm->getCurrentProfile()));
}


ConfigWidget::~ConfigWidget()
{
	QList<Profile*> oldProfiles = ProfileManager::getInstance()->getProfiles();

	QList<Profile*>::iterator it;

	for (it = profiles.begin() ; it != profiles.end() ; it++) {
		if (!oldProfiles.contains(*it)) {
			delete *it;
		}
	}

	int numProfileWidgets = ui.profileStackedWidget->count();

	for (int i = 0 ; i < numProfileWidgets ; i++) {
		ProfileConfigWidget* widget = (ProfileConfigWidget*) ui.profileStackedWidget->widget(0);
		ui.profileStackedWidget->removeWidget(widget);
		delete widget;
	}
}


void ConfigWidget::selectedProfileChanged(int index)
{
	ui.profileBox->setCurrentIndex(index);
	ui.profileStackedWidget->setCurrentIndex(index);
	ui.removeProfileButton->setEnabled(ui.profileBox->count() > 0);
}


void ConfigWidget::buttonClicked(QAbstractButton* button)
{
	QDialogButtonBox::StandardButton sbutton = ui.buttonBox->standardButton(button);

	if (sbutton == QDialogButtonBox::Cancel) {
		cancel();
	} else if (sbutton == QDialogButtonBox::Apply) {
		apply();
	}
}


void ConfigWidget::apply()
{
	QSettings settings;

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
	settings.setValue("gui/file_tree_auto_select", ui.fileTreeAutoSelectBox->isChecked());

	for (int i = 0 ; i < profiles.size() ; i++) {
		Profile* profile = profiles[i];
		ProfileConfigWidget* profileWidget = (ProfileConfigWidget*) ui.profileStackedWidget->widget(i);

		profile->setName(profileWidget->getProfileName());

		GameInfo info(profileWidget->getVersionMode(), File(profileWidget->getRootDirectory().toAscii().constData()));
		profile->setGameInfo(info);

		// ********** Set the new engine resources **********

		QLinkedList<QString> newResources;
		profileWidget->getResourceFiles(newResources);
		QLinkedList<QString>::iterator it;

		QLinkedList<File> newResFiles;
		for (it = newResources.begin() ; it != newResources.end() ; it++) {
			newResFiles << File(it->toLocal8Bit().constData());
		}

		bool hasChanges = profile->setResources(newResFiles);

		// ********** Set the new engine resources **********

		newResources.clear();
		profileWidget->getSearchResourceFiles(newResources);

		newResFiles.clear();
		for (it = newResources.begin() ; it != newResources.end() ; it++) {
			newResFiles << File(it->toLocal8Bit().constData());
		}

		hasChanges = profile->setSearchResources(newResFiles) ? true : hasChanges;

		QLinkedList<QString> datFiles;
		profileWidget->getDATFiles(datFiles);

		QLinkedList<File> newDATFiles;

		for (it = datFiles.begin() ; it != datFiles.end() ; it++) {
			newDATFiles << File(it->toLocal8Bit().constData());
		}

		profile->setDATFiles(newDATFiles);

		if (hasChanges) {
			profile->updateResourceIndex();
		}
	}

	ProfileManager* pm = ProfileManager::getInstance();
	pm->setProfiles(profiles);
	pm->setCurrentProfile(profiles[ui.profileBox->currentIndex()]);
	pm->saveProfiles();
	System::getInstance()->emitConfigurationChange();

	settings.sync();

	close();
}


void ConfigWidget::cancel()
{
	close();
}


void ConfigWidget::onNewProfile(bool)
{
	Profile* profile = new Profile(tr("New Profile"));
	profiles << profile;
	ui.profileBox->addItem(profile->getName(), QVariant::fromValue((void*) profile));
	ui.profileStackedWidget->addWidget(new ProfileConfigWidget(profile, ui.profileStackedWidget));
	ui.profileBox->setCurrentIndex(ui.profileBox->count()-1);
}


void ConfigWidget::onRemoveProfile(bool)
{
	int idx = ui.profileBox->currentIndex();
	Profile* profile = (Profile*) ui.profileBox->itemData(idx).value<void*>();
	profiles.removeOne(profile);
	ui.profileBox->removeItem(idx);
	ProfileConfigWidget* widget = (ProfileConfigWidget*) ui.profileStackedWidget->widget(idx);
	ui.profileStackedWidget->removeWidget(widget);
	delete widget;
}


