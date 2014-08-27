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

#ifndef RWBSGUIMODULE_H_
#define RWBSGUIMODULE_H_

#include "../../gui/GUIModule.h"
#include "../../gui/HexEditor.h"
#include "RWBSUtilityWidget.h"
#include "RWBSAnalyzerWidget.h"
#include <QAction>
#include <QDockWidget>


class RWBSGUIModule : public GUIModule {
	Q_OBJECT

public:
	static RWBSGUIModule* getInstance() { static RWBSGUIModule* inst = new RWBSGUIModule; return inst; }

public:
	void setEditor(HexEditor* editor) { this->editor = editor; }
	RWBSUtilityWidget* getUtilityWidget() { return utilWidget; }

private:
	RWBSGUIModule();

private:
	virtual void doInstall();
	virtual void doUninstall();

private slots:
	void gotoRequested();

private:
	QAction* gotoAction;
	HexEditor* editor;
	QDockWidget* utilDockWidget;
	QDockWidget* analyzerDockWidget;
	RWBSUtilityWidget* utilWidget;
};

#endif /* RWBSGUIMODULE_H_ */
