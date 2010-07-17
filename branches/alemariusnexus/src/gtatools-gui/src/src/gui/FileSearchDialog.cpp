/*
 * FileSearchDialog.cpp
 *
 *  Created on: 17.07.2010
 *      Author: alemariusnexus
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



FileSearchDialog::FileSearchDialog(QWidget* parent)
		: QDialog(parent)
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
	close();
}


void FileSearchDialog::onSearch(bool checked)
{
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

	Profile* profile = ProfileManager::getInstance()->getCurrentProfile();
	Profile::ResourceIterator rit;

	for (rit = profile->getResourceBegin() ; rit != profile->getResourceEnd() ; rit++) {
		File* resource = *rit;
		collectResults(*resource, &matcher, dirs, selectedFormats, results);
	}

	bool closeDialog = false;

	if (results.size() == 0) {
		QMessageBox::information(this, tr("No Match"), tr("No file matching your criteria was found!"));
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


void FileSearchDialog::collectResults(const File& file, StringMatcher* matcher, bool dirs, QLinkedList<FormatHandler*> handlers,
		QList<File*>& results)
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
			collectResults(*child, matcher, dirs, handlers, results);
			delete child;
		}

		delete it;
	}
}


