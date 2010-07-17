/*
 * ConfigWidget.cpp
 *
 *  Created on: 13.07.2010
 *      Author: alemariusnexus
 */

#include "ConfigWidget.h"
#include "../ProfileManager.h"
#include <qsettings.h>
#include <config.h>



ConfigWidget::ConfigWidget()
		: QWidget()
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
}


void ConfigWidget::onCancel(bool checked)
{
	close();
}


