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

#ifndef DEFAULTGUIMODULE_H_
#define DEFAULTGUIMODULE_H_

#include "GUIModule.h"
#include "MainWindow.h"
#include <qobject.h>
#include <qaction.h>
#include <qmenu.h>



class DefaultGUIModule : public GUIModule {
	Q_OBJECT

public:
	DefaultGUIModule();
	virtual ~DefaultGUIModule();
	virtual void buildFileTreeMenu(const File& file, QMenu& menu);

private:
	virtual void doInstall();
	virtual void doUninstall();

private slots:
	void onOpenSystemProgram(bool checked);
	void profileSwitchRequested(QAction* action);
	void settingsRequested(bool checked);
	void onAboutQt(bool checked);
	void onAbout(bool checked);
	void onFileOpen(bool checked);
	void onFileClose(bool checked);
	void fileOpened(const File& file);
	void fileClosed();
	void onSearchFile(bool checked);

private:
	QActionGroup* profileSwitchGroup;
	QAction* fileOpenAction;
	QAction* fileCloseAction;
	QAction* settingsAction;
	QAction* aboutQtAction;
	QAction* aboutAction;
	QAction* searchFileAction;
	QAction* systemOpenAction;
	File* contextFile;
	QMenu* profileSwitchMenu;
};

#endif /* DEFAULTGUIMODULE_H_ */
