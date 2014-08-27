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

#include "GUI.h"
#include "../System.h"
#include "../ProfileManager.h"
#include "../TaskFileFinder.h"
#include <QInputDialog>
#include <QMessageBox>



GUI* GUI::getInstance()
{
	static GUI* gui = new GUI;
	return gui;
}


File GUI::findFile(const QLinkedList<File>& rootFiles, FileFinder* finder, QWidget* parent)
{
	vector<File> files;

	QLinkedList<File>::const_iterator it;
	int numFiles = 0;

	for (it = rootFiles.begin() ; it != rootFiles.end() ; it++) {
		File file = *it;
		numFiles += file.getChildCount(true, true) + 1;
	}

	TaskFileFinder* proxyFinder = new TaskFileFinder(finder, tr("Searching files..."), numFiles, false);

	for (it = rootFiles.begin() ; it != rootFiles.end() ; it++) {
		File file = *it;

		if (file.isDirectoryOrArchiveDirectory()) {
			file.findChildren(*proxyFinder, files, true, true);
		} else {
			if (proxyFinder->matches(file)) {
				files.push_back(file);
			}
		}

		if (finder->isInterrupted()) {
			vector<File>::iterator it;
			delete proxyFinder;
			return File();
		}
	}

	delete proxyFinder;

	if (files.size() > 1) {
		QStringList options;
		vector<File>::iterator it;

		for (it = files.begin() ; it != files.end() ; it++) {
			File file = *it;
			options << file.getPath().toString().get();
		}

		bool ok;
		QString selected = QInputDialog::getItem(parent, tr("Multiple Matches"),
				tr("Please select the file you want to open:"), options, 0, false, &ok);

		if (ok) {
			File file(selected.toLocal8Bit().constData());
			return file;
		}
	} else if (files.size() == 1) {
		File file = *files.begin();
		return file;
	} else {
		QMessageBox::information(parent, tr("File Not Found"), tr("The requested file was not found!"));
	}

	return File();
}


File GUI::findFile(FileFinder* finder, QWidget* parent)
{
	Profile* profile = ProfileManager::getInstance()->getCurrentProfile();

	if (profile) {
		return findFile(profile->getSearchResources(), finder, parent);
	}

	return File();
}


bool GUI::findAndOpenFile(const QLinkedList<File>& rootFiles, FileFinder* finder, QWidget* parent)
{
	File file = findFile(rootFiles, finder, parent);

	if (!file.isNull()) {
		EntityOpenRequest req;
		req.setAttribute("type", "file");
		req.setAttribute("file", QString(file.getPath().toString().get()));
		System::getInstance()->openEntity(req);
		return true;
	}

	return false;
}


bool GUI::findAndOpenFile(FileFinder* finder, QWidget* parent)
{
	Profile* profile = ProfileManager::getInstance()->getCurrentProfile();

	if (profile) {
		return findAndOpenFile(profile->getSearchResources(), finder, parent);
	}

	return false;
}
