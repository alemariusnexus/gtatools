/*
 * DefaultGUIModule.h
 *
 *  Created on: 16.07.2010
 *      Author: alemariusnexus
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
