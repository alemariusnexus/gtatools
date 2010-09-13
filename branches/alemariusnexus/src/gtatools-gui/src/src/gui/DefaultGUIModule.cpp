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

#include "DefaultGUIModule.h"
#include <qdesktopservices.h>
#include <qurl.h>
#include <qactiongroup.h>
#include "../Profile.h"
#include "../ProfileManager.h"
#include "ConfigWidget.h"
#include <qmessagebox.h>
#include <qfiledialog.h>
#include "../System.h"
#include <gf_config.h>
#include "FileSearchDialog.h"
#include "VersionDialog.h"



DefaultGUIModule::DefaultGUIModule()
		: contextFile(NULL)
{
	System* sys = System::getInstance();

	fileOpenAction = new QAction(tr("Open..."), NULL);
	fileOpenAction->setShortcut(QKeySequence::Open);
	connect(fileOpenAction, SIGNAL(triggered(bool)), this, SLOT(onFileOpen(bool)));

	fileCloseAction = new QAction(tr("Close"), NULL);
	fileCloseAction->setShortcut(QKeySequence::Close);
	connect(fileCloseAction, SIGNAL(triggered(bool)), this, SLOT(onFileClose(bool)));
	fileCloseAction->setEnabled(false);

	searchFileAction = new QAction(tr("Search File..."), NULL);
	searchFileAction->setShortcut(QKeySequence::Find);
	connect(searchFileAction, SIGNAL(triggered(bool)), this, SLOT(onSearchFile(bool)));

	settingsAction = new QAction(tr("Settings..."), NULL);
	settingsAction->setShortcut(QKeySequence::Preferences);
	connect(settingsAction, SIGNAL(triggered(bool)), this, SLOT(settingsRequested(bool)));

	aboutQtAction = new QAction(tr("About Qt"), NULL);
	connect(aboutQtAction, SIGNAL(triggered(bool)), this, SLOT(onAboutQt(bool)));

	aboutAction = new QAction(tr("About"), NULL);
	connect(aboutAction, SIGNAL(triggered(bool)), this, SLOT(onAbout(bool)));

	versionInfoAction = new QAction(tr("Version Information"), NULL);
	connect(versionInfoAction, SIGNAL(triggered(bool)), this, SLOT(onVersionInfo(bool)));

	systemOpenAction = new QAction(tr("Execute System Program"), NULL);
	connect(systemOpenAction, SIGNAL(triggered(bool)), this, SLOT(onOpenSystemProgram(bool)));

	connect(sys, SIGNAL(fileOpened(const File&, const QHash<QString, QVariant>&)), this,
			SLOT(fileOpened(const File&, const QHash<QString, QVariant>&)));
	connect(sys, SIGNAL(currentFileClosed()), this, SLOT(fileClosed()));
}


DefaultGUIModule::~DefaultGUIModule()
{
	delete fileOpenAction;
	delete fileCloseAction;
	delete searchFileAction;
	delete settingsAction;
	delete aboutQtAction;
	delete aboutAction;
	delete versionInfoAction;
	delete systemOpenAction;

	if (contextFile) {
		delete contextFile;
	}

	QList<QAction*> actions = profileSwitchGroup->actions();
	for (int i = 0 ; i < actions.size() ; i++) {
		delete actions.at(i);
	}

	delete profileSwitchGroup;
}


void DefaultGUIModule::doInstall()
{
	QMenu* fileMenu = mainWindow->getFileMenu();
	QMenu* editMenu = mainWindow->getEditMenu();
	QMenu* settingsMenu = mainWindow->getSettingsMenu();
	QMenu* profileMenu = mainWindow->getProfileMenu();
	QMenu* helpMenu = mainWindow->getHelpMenu();

	fileOpenAction->setParent(mainWindow);
	fileCloseAction->setParent(mainWindow);
	searchFileAction->setParent(mainWindow);
	settingsAction->setParent(mainWindow);
	aboutQtAction->setParent(mainWindow);
	aboutAction->setParent(mainWindow);
	versionInfoAction->setParent(mainWindow);
	systemOpenAction->setParent(mainWindow);

	fileMenu->addAction(fileOpenAction);
	fileMenu->addAction(fileCloseAction);
	editMenu->addAction(searchFileAction);
	settingsMenu->addAction(settingsAction);
	helpMenu->addAction(aboutQtAction);
	helpMenu->addAction(aboutAction);
	helpMenu->addAction(versionInfoAction);

	profileSwitchMenu = new QMenu(tr("Switch"), profileMenu);
	profileMenu->addMenu(profileSwitchMenu);

	profileSwitchGroup = new QActionGroup(this);

	loadProfileSwitchMenu();

	ProfileManager* pm = ProfileManager::getInstance();

	connect(profileSwitchGroup, SIGNAL(triggered(QAction*)), this, SLOT(profileSwitchRequested(QAction*)));
	connect(pm, SIGNAL(profileAdded(Profile*)), this, SLOT(profileAdded(Profile*)));
	connect(pm, SIGNAL(profileRemoved(Profile*)), this, SLOT(profileRemoved(Profile*)));
}


void DefaultGUIModule::loadProfileSwitchMenu()
{
	QList<QAction*> actions = profileSwitchGroup->actions();
	QList<QAction*>::iterator it;

	for (it = actions.begin() ; it != actions.end() ; it++) {
		QAction* action = *it;
		profileSwitchGroup->removeAction(action);
		profileSwitchMenu->removeAction(action);
		delete action;
	}

	ProfileManager* pm = ProfileManager::getInstance();
	ProfileManager::ProfileIterator pit;

	Profile* currentProfile = pm->getCurrentProfile();

	QAction* noProfileAction = new QAction(tr("No Profile"), this);
	noProfileAction->setCheckable(true);
	noProfileAction->setData(-1);

	if (currentProfile == NULL) {
		noProfileAction->setChecked(true);
	}

	profileSwitchGroup->addAction(noProfileAction);
	profileSwitchMenu->addAction(noProfileAction);

	profileSwitchMenu->addSeparator();

	for (pit = pm->getProfileBegin() ; pit != pm->getProfileEnd() ; pit++) {
		Profile* profile = *pit;

		QAction* action = new QAction(profile->getName(), this);
		action->setCheckable(true);
		action->setData(pm->indexOfProfile(profile));

		if (profile == currentProfile) {
			action->setChecked(true);
		}

		profileSwitchGroup->addAction(action);
		profileSwitchMenu->addAction(action);
	}
}


void DefaultGUIModule::doUninstall()
{
	QMenu* fileMenu = mainWindow->getFileMenu();
	QMenu* editMenu = mainWindow->getEditMenu();
	QMenu* settingsMenu = mainWindow->getSettingsMenu();
	QMenu* profileMenu = mainWindow->getProfileMenu();
	QMenu* helpMenu = mainWindow->getHelpMenu();

	fileOpenAction->setParent(NULL);
	fileCloseAction->setParent(NULL);
	searchFileAction->setParent(NULL);
	settingsAction->setParent(NULL);
	aboutQtAction->setParent(NULL);
	aboutAction->setParent(NULL);
	versionInfoAction->setParent(NULL);

	fileMenu->removeAction(fileOpenAction);
	fileMenu->removeAction(fileCloseAction);
	editMenu->removeAction(searchFileAction);
	settingsMenu->removeAction(settingsAction);
	helpMenu->removeAction(aboutQtAction);
	helpMenu->removeAction(aboutAction);
	helpMenu->removeAction(versionInfoAction);

	ProfileManager* pm = ProfileManager::getInstance();

	disconnect(pm, SIGNAL(profileAdded(Profile*)), this, SLOT(profileAdded(Profile*)));
	disconnect(pm, SIGNAL(profileRemoved(Profile*)), this, SLOT(profileRemoved(Profile*)));

	delete profileSwitchMenu;
}


void DefaultGUIModule::profileSwitchRequested(QAction* action)
{
	ProfileManager* pm = ProfileManager::getInstance();
	int index = action->data().toInt();

	Profile* profile = NULL;

	if (index != -1) {
		profile = pm->getProfile(action->data().toInt());
	}

	pm->setCurrentProfile(profile);
}


void DefaultGUIModule::settingsRequested(bool checked)
{
	ConfigWidget* cfgWidget = new ConfigWidget;
	cfgWidget->show();
}


void DefaultGUIModule::buildFileTreeMenu(const File& file, QMenu& menu)
{
	if (contextFile) {
		delete contextFile;
		contextFile = NULL;
	}

	menu.addAction(systemOpenAction);
	contextFile = new File(file);
}


void DefaultGUIModule::onOpenSystemProgram(bool checked)
{
	QDesktopServices::openUrl(QUrl(QString("file://%1").arg(contextFile->getPath()->toString())));
}


void DefaultGUIModule::onAboutQt(bool checked)
{
	QMessageBox::aboutQt(mainWindow, tr("About Qt"));
}


void DefaultGUIModule::onAbout(bool checked)
{
	QString aboutText = tr("AboutText").arg(GTATOOLS_VERSION);
	QMessageBox::about(mainWindow, tr("About GTATools GUI Tool"), aboutText);
}


void DefaultGUIModule::onVersionInfo(bool checked)
{
	VersionDialog dialog(mainWindow);
	dialog.exec();
}


void DefaultGUIModule::onFileOpen(bool checked)
{
	QString fname = QFileDialog::getOpenFileName(mainWindow, tr("Select file"));

	if (!fname.isNull()) {
		System::getInstance()->openFile(File(fname.toLocal8Bit().constData()));
	}
}


void DefaultGUIModule::onFileClose(bool checked)
{
	System::getInstance()->closeCurrentFile();
}


void DefaultGUIModule::fileOpened(const File& file, const QHash<QString, QVariant>& data)
{
	fileCloseAction->setEnabled(true);
}


void DefaultGUIModule::fileClosed()
{
	fileCloseAction->setEnabled(false);
}


void DefaultGUIModule::onSearchFile(bool checked)
{
	FileSearchDialog dialog(mainWindow);
	dialog.exec();
}


void DefaultGUIModule::profileAdded(Profile* profile)
{
	loadProfileSwitchMenu();
}


void DefaultGUIModule::profileRemoved(Profile* profile)
{
	loadProfileSwitchMenu();
}
