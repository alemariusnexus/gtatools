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

#ifndef DFFGUIMODULE_H_
#define DFFGUIMODULE_H_

#include "../../gui/GUIModule.h"
#include <QObject>
#include <QAction>


class DFFWidget;



class DFFGUIModule : public GUIModule {
	Q_OBJECT

public:
	static DFFGUIModule* getInstance() { static DFFGUIModule* inst = new DFFGUIModule; return inst; }

signals:
	void dumpRequested();

private:
	DFFGUIModule();
	~DFFGUIModule();
	virtual void doInstall();
	virtual void doUninstall();

private slots:
	void dumpRequestedSlot(bool) { emit dumpRequested(); }

private:
	QAction* dumpAction;
};

#endif /* DFFGUIMODULE_H_ */
