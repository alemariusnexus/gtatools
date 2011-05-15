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

#include "FileSearchDialog.h"
#include <gtatools-gui/config.h>
#include "../ProfileManager.h"
#include "../Profile.h"
#include "../System.h"
#include "../RegexFileFinder.h"
#include "../TaskFileFinder.h"
#include "GUI.h"
#include <gtaformats/util/DefaultFileFinder.h>
#include <QtCore/QRegExp>
#include <QtCore/QSettings>
#include <QtCore/QLinkedList>




FileSearchDialog::FileSearchDialog(QWidget* parent)
		: QDialog(parent), finder(NULL)
{
	ui.setupUi(this);

	connect(ui.cancelButton, SIGNAL(clicked(bool)), this, SLOT(onCancel(bool)));
	connect(ui.searchButton, SIGNAL(clicked(bool)), this, SLOT(onSearch(bool)));
}


void FileSearchDialog::onCancel(bool checked)
{
	if (finder) {
		finder->interrupt();
	}

	close();
}


void FileSearchDialog::onSearch(bool checked)
{
	ui.searchButton->setEnabled(false);

	if (ui.regexBox->isChecked()) {
		QRegExp::PatternSyntax syntax;

		QSettings settings;

		QString cfgSyntax = settings.value("main/regex_syntax", "wildcard").toString();

		if (cfgSyntax == "wildcard") {
			syntax = QRegExp::WildcardUnix;
		} else if (cfgSyntax == "full") {
			syntax = QRegExp::RegExp;
		} else {
			syntax = QRegExp::WildcardUnix;
		}

		QRegExp regex(ui.nameField->text(), ui.caseBox->isChecked() ? Qt::CaseSensitive
				: Qt::CaseInsensitive, syntax);
		finder = new RegexFileFinder(regex, ui.exactBox->isChecked());
	} else {
		finder = new DefaultFileFinder(ui.nameField->text().toLocal8Bit().constData(),
				ui.caseBox->isChecked(), ui.exactBox->isChecked());
	}

	Profile* profile = ProfileManager::getInstance()->getCurrentProfile();

	bool closeDialog = true;

	if (profile) {
		QLinkedList<File*> rootFiles = profile->getResources();
		closeDialog = GUI::getInstance()->findAndOpenFile(rootFiles, finder, this);
	}

	delete finder;
	finder = NULL;

	if (closeDialog) {
		close();
	} else {
		ui.searchButton->setEnabled(true);
	}
}


