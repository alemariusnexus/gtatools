/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#ifndef GUIMODULE_H_
#define GUIMODULE_H_

#include <QtCore/QObject>
#include <QtGui/QMenu>
#include <gtaformats/util/File.h>
#include <QtCore/QLinkedList>


class MainWindow;



class GUIModule : public QObject {
	Q_OBJECT

public:
	GUIModule() : mainWindow(NULL), installCount(0) {};
	virtual ~GUIModule();
	virtual void buildFileTreeMenu(const QLinkedList<File*>& files, QMenu& menu) {}
	int getInstallCount() const { return installCount; }

private:
	bool install(MainWindow* mw);
	bool uninstall();
	void forceUninstall();

private:
	virtual void doInstall() = 0;
	virtual void doUninstall() = 0;

protected:
	MainWindow* mainWindow;

private:
	int installCount;

	friend class System;
};

#endif /* GUIMODULE_H_ */
