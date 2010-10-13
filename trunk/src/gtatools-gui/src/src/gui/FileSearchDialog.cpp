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

#include "FileSearchDialog.h"
#include "../formats/FormatManager.h"
#include "../formats/FormatHandler.h"
#include <qlinkedlist.h>
#include "../StringMatcher.h"
#include "../ProfileManager.h"
#include "../Profile.h"
#include <qmessagebox.h>
#include <qinputdialog.h>
#include "../System.h"
#include <qprogressdialog.h>




FileSearchDialog::FileSearchDialog(QWidget* parent)
		: QDialog(parent), cancelled(false)
{
	ui.setupUi(this);

	QLinkedList<FormatHandler*> handlers = FormatManager::getInstance()->getHandlers();

	ui.typeList->addItem(tr("Directory"));

	QLinkedList<FormatHandler*>::iterator it;
	for (it = handlers.begin() ; it != handlers.end() ; it++) {
		FormatHandler* handler = *it;
		ui.typeList->addItem(handler->getFormatName());
	}

	ui.typeList->selectAll();

	connect(ui.cancelButton, SIGNAL(clicked(bool)), this, SLOT(onCancel(bool)));
	connect(ui.searchButton, SIGNAL(clicked(bool)), this, SLOT(onSearch(bool)));
}


void FileSearchDialog::onCancel(bool checked)
{
	cancelled = true;
	close();
}


void FileSearchDialog::onSearch(bool checked)
{
	ui.searchButton->setEnabled(false);

	StringMatcher::flags flags = 0;

	if (ui.regexBox->isChecked()) {
		flags |= STRING_MATCHER_REGEX;
	}
	if (ui.exactBox->isChecked()) {
		flags |= STRING_MATCHER_EXACTMATCH;
	}
	if (!ui.caseBox->isChecked()) {
		flags |= STRING_MATCHER_CASEINSENSITIVE;
	}

	StringMatcher matcher(ui.nameField->text(), flags);

	QLinkedList<FormatHandler*> handlers = FormatManager::getInstance()->getHandlers();
	QLinkedList<FormatHandler*> selectedFormats;

	bool dirs = ui.typeList->item(0)->isSelected();

	int i = 1;
	QLinkedList<FormatHandler*>::iterator it;
	for (it = handlers.begin() ; it != handlers.end() ; it++, i++) {
		QListWidgetItem* item = ui.typeList->item(i);
		FormatHandler* handler = *it;

		if (item->isSelected()) {
			selectedFormats << handler;
		}
	}

	QList<File*> results;

	int numFiles = 0;
	int filesDone = 0;

	Profile* profile = ProfileManager::getInstance()->getCurrentProfile();
	Profile::ResourceIterator rit;

	System* sys = System::getInstance();

	for (rit = profile->getResourceBegin() ; rit != profile->getResourceEnd() ; rit++) {
		File* resource = *rit;
		numFiles += resource->getChildCount(true, true)+1; // The resource file itself, too.
	}

	//sys->startTask(0, 100, tr("Searching files..."));
	Task* task = sys->createTask();
	task->start(0, 100, tr("Searching files..."));

	for (rit = profile->getResourceBegin() ; rit != profile->getResourceEnd() ; rit++) {
		File* resource = *rit;
		if (!collectResults(*resource, &matcher, dirs, selectedFormats, results, numFiles, filesDone, task)) {
			delete task;

			QList<File*>::iterator resIt;
			for (resIt = results.begin() ; resIt != results.end() ; resIt++) {
				delete *resIt;
			}

			return;
		}
	}

	//sys->endTask();
	delete task;

	bool closeDialog = false;

	if (results.size() == 0) {
		QMessageBox::information(this, tr("No Match"), tr("No file matching your criteria was found!"));
		ui.searchButton->setEnabled(true);
	} else if (results.size() == 1) {
		System::getInstance()->openFile(*results.at(0));
		closeDialog = true;
	} else {
		QStringList userItems;

		QList<File*>::iterator resIt;
		for (resIt = results.begin() ; resIt != results.end() ; resIt++) {
			File* file = *resIt;
			userItems << file->getPath()->toString();
		}

		bool okSelected;
		QString sel = QInputDialog::getItem(this, tr("Multiple Matches"), tr("Please select the file you want to open:"),
				userItems, 0, false, &okSelected);

		if (okSelected) {
			File* file = results.at(userItems.indexOf(sel));
			System::getInstance()->openFile(*file);
			closeDialog = true;
		} else {
			ui.searchButton->setEnabled(true);
		}
	}

	QList<File*>::iterator resIt;
	for (resIt = results.begin() ; resIt != results.end() ; resIt++) {
		delete *resIt;
	}

	if (closeDialog) {
		close();
	}
}


bool FileSearchDialog::collectResults(const File& file, StringMatcher* matcher, bool dirs, QLinkedList<FormatHandler*> handlers,
		QList<File*>& results, int filesMax, int& filesDone, Task* task)
{
	bool matches = false;

	if (!file.isDirectory()) {
		QLinkedList<FormatHandler*>::iterator it;

		for (it = handlers.begin() ; it != handlers.end() ; it++) {
			FormatHandler* handler = *it;

			if (handler->hasFileFormat(file)) {
				matches = true;
				break;
			}
		}

		matches = true;
	} else if (dirs) {
		matches = true;
	}

	if (matches) {
		if (matcher->matches(file.getPath()->getFileName())) {
			results << new File(file);
		} else {
			matches = false;
		}
	}

	if (file.isDirectory()  ||  file.isArchiveFile()) {
		FileIterator* it = file.getIterator();
		File* child;

		while ((child = it->next())  !=  NULL) {
			if (!collectResults(*child, matcher, dirs, handlers, results, filesMax, filesDone, task)) {
				delete child;
				return false;
			}
			delete child;
		}

		delete it;
	}

	filesDone++;

	qApp->processEvents();

	if (cancelled) {
		return false;
	}

	if (filesMax < 100) {
		System* sys = System::getInstance();
		//sys->updateTaskValue((int) (((float)filesDone / (float) filesMax)*100.0f));
		task->update((int) (((float)filesDone / (float) filesMax)*100.0f));
	} else {
		if (filesDone%(filesMax / 100) == 0) {
			System* sys = System::getInstance();
			//sys->updateTaskValue(sys->getTaskValue()+1);
			task->update(task->getValue()+1);
		}
	}

	return true;
}


