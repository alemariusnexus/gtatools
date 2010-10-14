/*
 * GUI.cpp
 *
 *  Created on: 13.10.2010
 *      Author: alemariusnexus
 */

#include "GUI.h"
#include "../System.h"
#include "../ProfileManager.h"
#include "../TaskFileFinder.h"
#include <QtGui/QInputDialog>
#include <QtGui/QMessageBox>



GUI* GUI::getInstance()
{
	static GUI* gui = new GUI;
	return gui;
}


File* GUI::findFile(const QLinkedList<File*>& rootFiles, FileFinder* finder, QWidget* parent)
{
	vector<File*> files;

	QLinkedList<File*>::const_iterator it;
	int numFiles = 0;

	for (it = rootFiles.begin() ; it != rootFiles.end() ; it++) {
		File* file = *it;
		numFiles += file->getChildCount(true, true) + 1;
	}

	TaskFileFinder* proxyFinder = new TaskFileFinder(finder, tr("Searching files..."), numFiles, false);

	for (it = rootFiles.begin() ; it != rootFiles.end() ; it++) {
		File* file = *it;
		file->findChildren(*proxyFinder, files, true, true);

		if (finder->isInterrupted()) {
			vector<File*>::iterator it;
			for (it = files.begin() ; it != files.end() ; it++) {
				delete *it;
			}
			delete proxyFinder;
			return NULL;
		}
	}

	delete proxyFinder;

	System* sys = System::getInstance();

	if (files.size() > 1) {
		QStringList options;
		vector<File*>::iterator it;

		for (it = files.begin() ; it != files.end() ; it++) {
			File* file = *it;
			options << file->getPath()->toString();
			delete file;
		}

		bool ok;
		QString selected = QInputDialog::getItem(parent, tr("Multiple Matches"),
				tr("Please select the file you want to open:"), options, 0, false, &ok);

		if (ok) {
			File* file = new File(selected.toLocal8Bit().constData());
			return file;
		}
	} else if (files.size() == 1) {
		File* file = *files.begin();
		return file;
	} else {
		QMessageBox::information(parent, tr("File Not Found"), tr("The requested file was not found!"));
	}

	return NULL;
}


File* GUI::findFile(FileFinder* finder, QWidget* parent)
{
	Profile* profile = ProfileManager::getInstance()->getCurrentProfile();

	if (profile) {
		QLinkedList<File*> rootFiles;
		Profile::ResourceIterator it;

		for (it = profile->getResourceBegin() ; it != profile->getResourceEnd() ; it++) {
			rootFiles << *it;
		}

		return findFile(rootFiles, finder, parent);
	}

	return NULL;
}


bool GUI::findAndOpenFile(const QLinkedList<File*>& rootFiles, FileFinder* finder, QWidget* parent)
{
	File* file = findFile(rootFiles, finder, parent);

	if (file) {
		FileOpenRequest req(*file);
		System::getInstance()->openFile(req);
		delete file;
		return true;
	}

	return false;
}


bool GUI::findAndOpenFile(FileFinder* finder, QWidget* parent)
{
	Profile* profile = ProfileManager::getInstance()->getCurrentProfile();

	if (profile) {
		QLinkedList<File*> rootFiles;
		Profile::ResourceIterator it;

		for (it = profile->getResourceBegin() ; it != profile->getResourceEnd() ; it++) {
			rootFiles << *it;
		}

		return findAndOpenFile(rootFiles, finder, parent);
	}

	return false;
}
