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

#ifndef IMGGUIMODULE_H_
#define IMGGUIMODULE_H_

#include "../../gui/GUIModule.h"
#include <qaction.h>
#include <qhash.h>
#include <qstring.h>
#include <qvariant.h>


class IMGGUIModule : public GUIModule {
	Q_OBJECT

public:
	IMGGUIModule();
	virtual ~IMGGUIModule();
	virtual void buildFileTreeMenu(const QLinkedList<File*>& files, QMenu& menu);
	void openFileChanged(const File& file);

private:
	virtual void doInstall();
	virtual void doUninstall();
	void extract(const QLinkedList<File*>& files);

private slots:
	void onExtract(bool);
	void onContextExtract(bool);
	void currentFileChanged(File* file, File* prev);

private:
	QAction* extractAction;
	QAction* contextExtractAction;
	//File* currentContextFile;
	QLinkedList<File*> currentContextFiles;
	bool menuExists;
};

#endif /* IMGGUIMODULE_H_ */
