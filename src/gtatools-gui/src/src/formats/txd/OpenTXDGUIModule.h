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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef OPENTXDGUIMODULE_H_
#define OPENTXDGUIMODULE_H_

#include "../../gui/GUIModule.h"
#include <QtGui/QAction>
#include <QtCore/QLinkedList>


class TXDWidget;


struct ImageFormat
{
	QString* name;
	QLinkedList<QString*>* extensions;
};


class OpenTXDGUIModule : public GUIModule {
	Q_OBJECT

public:
	OpenTXDGUIModule(TXDWidget* txdWidget);
	~OpenTXDGUIModule();

private:
	virtual void doInstall();
	virtual void doUninstall();

private slots:
	void onExtract(bool checked);

private:
	QLinkedList<ImageFormat*> supportedImageFormats;
	TXDWidget* txdWidget;
	QAction* extractAction;
};

#endif /* OPENTXDGUIMODULE_H_ */
