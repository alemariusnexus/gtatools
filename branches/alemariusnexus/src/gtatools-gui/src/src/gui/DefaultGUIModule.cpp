/*
 * DefaultGUIModule.cpp
 *
 *  Created on: 16.07.2010
 *      Author: alemariusnexus
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



DefaultGUIModule::DefaultGUIModule(MainWindow* mw)
		: GUIModule(mw), contextFile(NULL)
{
	System* sys = System::getInstance();

	QMenu* fileMenu = mw->getFileMenu();
	QMenu* editMenu = mw->getEditMenu();
	QMenu* settingsMenu = mw->getSettingsMenu();
	QMenu* profileMenu = mw->getProfileMenu();
	QMenu* helpMenu = mw->getHelpMenu();

	fileOpenAction = new QAction(tr("Open..."), mw);
	fileOpenAction->setShortcut(QKeySequence::Open);
	connect(fileOpenAction, SIGNAL(triggered(bool)), this, SLOT(onFileOpen(bool)));
	fileMenu->addAction(fileOpenAction);

	fileCloseAction = new QAction(tr("Close"), mw);
	fileCloseAction->setShortcut(QKeySequence::Close);
	connect(fileCloseAction, SIGNAL(triggered(bool)), this, SLOT(onFileClose(bool)));
	fileMenu->addAction(fileCloseAction);
	fileCloseAction->setEnabled(false);

	searchFileAction = new QAction(tr("Search File..."), mw);
	searchFileAction->setShortcut(QKeySequence::Find);
	connect(searchFileAction, SIGNAL(triggered(bool)), this, SLOT(onSearchFile(bool)));
	editMenu->addAction(searchFileAction);

	settingsAction = new QAction(tr("Settings..."), mw);
	settingsAction->setShortcut(QKeySequence::Preferences);
	connect(settingsAction, SIGNAL(triggered(bool)), this, SLOT(settingsRequested(bool)));
	settingsMenu->addAction(settingsAction);

	aboutQtAction = new QAction(tr("About Qt"), mw);
	connect(aboutQtAction, SIGNAL(triggered(bool)), this, SLOT(onAboutQt(bool)));
	helpMenu->addAction(aboutQtAction);

	aboutAction = new QAction(tr("About"), mw);
	connect(aboutAction, SIGNAL(triggered(bool)), this, SLOT(onAbout(bool)));
	helpMenu->addAction(aboutAction);

	profileSwitchMenu = new QMenu(tr("Switch"), profileMenu);
	profileMenu->addMenu(profileSwitchMenu);

	systemOpenAction = new QAction(tr("Execute System Program"), mw);
	connect(systemOpenAction, SIGNAL(triggered(bool)), this, SLOT(onOpenSystemProgram(bool)));

	ProfileManager* pm = ProfileManager::getInstance();
	Profile* currentProfile = pm->getCurrentProfile();

	ProfileManager::ProfileIterator it;

	profileSwitchGroup = new QActionGroup(this);

	for (it = pm->getProfileBegin() ; it != pm->getProfileEnd() ; it++) {
		Profile* profile = *it;

		QAction* action = new QAction(profile->getName(), this);
		action->setCheckable(true);
		action->setData(pm->indexOfProfile(profile));

		if (profile == currentProfile) {
			action->setChecked(true);
		}

		profileSwitchGroup->addAction(action);
		profileSwitchMenu->addAction(action);
	}

	connect(profileSwitchGroup, SIGNAL(triggered(QAction*)), this, SLOT(profileSwitchRequested(QAction*)));
	connect(sys, SIGNAL(fileOpened(const File&)), this, SLOT(fileOpened(const File&)));
	connect(sys, SIGNAL(currentFileClosed()), this, SLOT(fileClosed()));

}


DefaultGUIModule::~DefaultGUIModule()
{
	delete systemOpenAction;

	if (contextFile) {
		delete contextFile;
	}

	delete settingsAction;
	delete fileOpenAction;
	delete fileCloseAction;

	QList<QAction*> actions = profileSwitchGroup->actions();
	for (int i = 0 ; i < actions.size() ; i++) {
		delete actions.at(i);
	}

	delete profileSwitchGroup;
}


void DefaultGUIModule::profileSwitchRequested(QAction* action)
{
	ProfileManager* pm = ProfileManager::getInstance();
	Profile* profile = pm->getProfile(action->data().toInt());
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


void DefaultGUIModule::fileOpened(const File& file)
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
