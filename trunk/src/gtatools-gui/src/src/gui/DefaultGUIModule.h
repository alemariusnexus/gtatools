/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#ifndef DEFAULTGUIMODULE_H_
#define DEFAULTGUIMODULE_H_

#include "GUIModule.h"
#include "MainWindow.h"
#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QUndoGroup>
#include "../EntityOpenRequest.h"
#include "FileTree.h"
#include <QtGui/QDockWidget>
#include "LogConsole.h"



class DefaultGUIModule : public GUIModule {
	Q_OBJECT

public:
	DefaultGUIModule();
	virtual ~DefaultGUIModule();
	virtual void buildFileTreeMenu(const QLinkedList<File*>& files, QMenu& menu);

private:
	virtual void doInstall();
	virtual void doUninstall();
	void loadProfileSwitchMenu();

private slots:
	void onOpenSystemProgram(bool checked);
	void profileSwitchRequested(QAction* action);
	void settingsRequested(bool checked);
	void onAboutQt(bool checked);
	void onAbout(bool checked);
	void onVersionInfo(bool checked);
	void onFileOpen(bool checked);
	void onFileClose(bool checked);
	void onFileSave(bool checked);
	void onFileSaveAs(bool checked);
	void entityOpened(DisplayedEntity* ent);
	void entityClosed(DisplayedEntity* ent);
	void currentEntityChanged(DisplayedEntity* prev, DisplayedEntity* current);
	void entityHasUnsavedChangesStateChanged(bool hasChanges);
	void onSearchFile(bool checked);
	void profileAdded(Profile* profile);
	void profileRemoved(Profile* profile);
	void profilesLoaded();
	void profileNameChanged(const QString& oldName, const QString& newName);
	void currentProfileChanged(Profile* oldProfile, Profile* newProfile);
	void onBuildPVS(bool checked);
	void entityCurrentEditHandlerChanged(DisplayedEntityHandler* oldHandler, DisplayedEntityHandler* newHandler);

private:
	LogConsole* logConsole;
	QActionGroup* profileSwitchGroup;
	QAction* fileOpenAction;
	QAction* fileCloseAction;
	QAction* fileSaveAction;
	QAction* fileSaveAsAction;
	QAction* settingsAction;
	QAction* aboutQtAction;
	QAction* aboutAction;
	QAction* versionInfoAction;
	QAction* searchFileAction;
	QAction* systemOpenAction;
	QAction* pvsAction;
	QAction* undoAction;
	QAction* redoAction;
	QMenu* profileSwitchMenu;
	QLinkedList<File*> contextFiles;
	QDockWidget* logConsoleDock;
	QUndoGroup undoGroup;
	QUndoStack dummyUndoStack;
	QMap<DisplayedEntity*, DisplayedEntityHandler*> entityLastReleasedEditLockHandlers;

	FileTree* fileTree;
	QDockWidget* fileTreeDock;
};

#endif /* DEFAULTGUIMODULE_H_ */
