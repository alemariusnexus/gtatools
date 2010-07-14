/*
 * ConfigWidget.cpp
 *
 *  Created on: 13.07.2010
 *      Author: alemariusnexus
 */

#include "ConfigWidget.h"
#include "../ProfileManager.h"
#include <qsettings.h>



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
}


void ConfigWidget::selectedProfileChanged(int index)
{

	Profile* profile = ProfileManager::getInstance()->getProfile(index);
	ui.profileWidget->displayProfile(profile);
}


void ConfigWidget::onApply(bool checked)
{
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
}


void ConfigWidget::onCancel(bool checked)
{
	close();
}


