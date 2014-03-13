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

#ifndef IDEGUIMODULE_H_
#define IDEGUIMODULE_H_

#include "../../gui/GUIModule.h"
#include "../../Profile.h"
#include <QtGui/QAction>



class IDEGUIModule : public GUIModule {
	Q_OBJECT

public:
	static IDEGUIModule* getInstance() { static IDEGUIModule* inst = new IDEGUIModule; return inst; }

public:
	IDEGUIModule();
	virtual ~IDEGUIModule();

private:
	virtual void doInstall();
	virtual void doUninstall();

private slots:
	void currentProfileChanged(Profile* oldProfile, Profile* newProfile);
	void onFindDefinition(bool checked);

private:
	QAction* findDefinitionAction;
};

#endif /* IDEGUIMODULE_H_ */
