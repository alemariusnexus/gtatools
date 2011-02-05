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

#ifndef COLGUIMODULE_H_
#define COLGUIMODULE_H_

#include "../../gui/GUIModule.h"
#include <QtGui/QAction>


class COLGUIModule : public GUIModule {
	Q_OBJECT

public:
	static COLGUIModule* getInstance() { static COLGUIModule inst; return &inst; }

signals:
	void wireframePropertyChanged(bool wireframe);

private slots:
	void wireframePropertyChangedSlot(bool wireframe);

private:
	COLGUIModule();
	virtual void doInstall();
	virtual void doUninstall();

private:
	QAction* wireframeAction;
};

#endif /* COLGUIMODULE_H_ */
