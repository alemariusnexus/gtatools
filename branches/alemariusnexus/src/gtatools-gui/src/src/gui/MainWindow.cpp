/*
 * MainWindow.cpp
 *
 *  Created on: 11.07.2010
 *      Author: alemariusnexus
 */

#include "MainWindow.h"
#include "FileItemModel.h"
#include "../ProfileManager.h"
#include "../formats/FormatManager.h"
#include "../formats/FormatHandler.h"
#include <qboxlayout.h>
#include <qlist.h>
#include <qsettings.h>
#include "ConfigWidget.h"
#include <qmenu.h>
#include <config.h>
#include "../formats/impl/TXDGUIModule.h"



MainWindow::MainWindow()
{
	ui.setupUi(this);
	setWindowTitle(tr("ProgramBaseName"));
	ui.mainSplitter->setSizes(QList<int>() << width()/3 << width()/3*2);
}


void MainWindow::initialize()
{
	ProfileManager* pm = ProfileManager::getInstance();
	Profile* currentProfile = pm->getCurrentProfile();

	ProfileManager::ProfileIterator it;

	QActionGroup* switchProfileGroup = new QActionGroup(this);

	for (it = pm->getProfileBegin() ; it != pm->getProfileEnd() ; it++) {
		Profile* profile = *it;

		QAction* action = new QAction(profile->getName(), this);
		action->setCheckable(true);
		action->setData(pm->indexOfProfile(profile));

		if (profile == currentProfile) {
			action->setChecked(true);
		}

		switchProfileGroup->addAction(action);
		ui.menuSwitchProfile->addAction(action);
	}

	currentProfileChanged(NULL, currentProfile);

	connect(ui.fileTree, SIGNAL(activated(const QModelIndex&)), this, SLOT(fileSelectedInTree(const QModelIndex&)));
	connect(ui.actionSettings, SIGNAL(triggered(bool)), this, SLOT(settingsRequested(bool)));
	connect(switchProfileGroup, SIGNAL(triggered(QAction*)), this, SLOT(profileSwitchRequested(QAction*)));
	connect(pm, SIGNAL(currentProfileChanged(Profile*, Profile*)), this,
			SLOT(currentProfileChanged(Profile*, Profile*)));

	globalModules << new TXDGUIModule();

	QLinkedList<GUIModule*>::iterator mit;
	for (mit = globalModules.begin() ; mit != globalModules.end() ; mit++) {
		(*mit)->apply(this);
	}

	QSettings settings(CONFIG_FILE, QSettings::IniFormat);

	resize(settings.value("gui/mainwindow_size", size()).toSize());
}


MainWindow::~MainWindow()
{
	QLinkedList<GUIModule*>::iterator it;
	for (it = globalModules.begin() ; it != globalModules.end() ; it++) {
		(*it)->remove(this);
	}

	QSettings settings(CONFIG_FILE, QSettings::IniFormat);
	settings.setValue("gui/mainwindow_size", size());
}


void MainWindow::fileSelectedInTree(const QModelIndex& index)
{
	if (!index.isValid()) {
		return;
	}

	File* file = static_cast<StaticFile*>(index.internalPointer())->getFile();
	openFile(*file);
}


void MainWindow::openFile(const File& file)
{
	/*if (currentFileModule) {
		currentFileModule->remove(this);
		delete currentFileModule;
		currentFileModule = NULL;
	}*/

	QLinkedList<GUIModule*>::iterator it;
	for (it = currentFileModules.begin() ; it != currentFileModules.end() ; it++) {
		(*it)->remove(this);
		delete *it;
	}
	currentFileModules.clear();

	ui.fileNameLabel->setText(QString(file.getPath()->getFileName()));

	if (file.isDirectory()) {
		ui.fileTypeLabel->setText(QString(tr("Directory")));
		ui.fileSizeLabel->setText(QString(tr("%1 files")).arg(file.getChildCount()));
	} else {
		File::filesize size = file.getSize();

		if (size > 1000000) {
			ui.fileSizeLabel->setText(QString(tr("%1MB")).arg(size/1000000.0f));
		} else if (size > 1000) {
			ui.fileSizeLabel->setText(QString(tr("%1kB")).arg(size/1000.0f));
		} else {
			ui.fileSizeLabel->setText(QString(tr("%1B")).arg(size));
		}

		FormatHandler* descHandler = FormatManager::getInstance()->getHandler(file, FORMAT_HANDLER_DESCRIBE, true);

		if (!descHandler) {
			ui.fileTypeLabel->setText(QString(tr("Unrecognized File")));
		} else {
			ui.fileTypeLabel->setText(descHandler->getFileFormatName(file, true));
		}

		QLinkedList<FormatHandler*> modHandlers = FormatManager::getInstance()->getHandlers(file, FORMAT_HANDLER_GUIMODULE, true);
		QLinkedList<FormatHandler*>::iterator fhit;

		for (fhit = modHandlers.begin() ; fhit != modHandlers.end() ; fhit++) {
			FormatHandler* handler = *fhit;
			GUIModule* mod = handler->createGUIModuleForFile(file);

			if (mod) {
				currentFileModules << mod;
				mod->apply(this);
			}
		}
	}
}


void MainWindow::currentProfileChanged(Profile* oldProfile, Profile* newProfile)
{
	FileItemModel* model = new FileItemModel(newProfile);

	setWindowTitle(QString(tr("GTATools GUI Tool - %1")).arg(newProfile->getName()));

	if (oldProfile != NULL) {
		FileItemModel* oldModel = (FileItemModel*) ui.fileTree->model();
		ui.fileTree->setModel(model);
		delete oldModel;

		disconnect(oldProfile, SIGNAL(changed()), this, SLOT(currentProfileContentChanged()));
	} else {
		ui.fileTree->setModel(model);
	}

	connect(newProfile, SIGNAL(changed()), this, SLOT(currentProfileContentChanged()));
}


void MainWindow::currentProfileContentChanged()
{
	Profile* profile = ProfileManager::getInstance()->getCurrentProfile();

	setWindowTitle(QString(tr("GTATools GUI Tool - %1")).arg(profile->getName()));

	FileItemModel* oldModel = (FileItemModel*) ui.fileTree->model();
	FileItemModel* model = new FileItemModel(profile);
	ui.fileTree->setModel(model);
	delete oldModel;
}


void MainWindow::settingsRequested(bool checked)
{
	ConfigWidget* cfgWidget = new ConfigWidget;
	cfgWidget->show();
}


void MainWindow::profileSwitchRequested(QAction* action)
{
	ProfileManager* pm = ProfileManager::getInstance();
	Profile* profile = pm->getProfile(action->data().toInt());
	pm->setCurrentProfile(profile);
}

