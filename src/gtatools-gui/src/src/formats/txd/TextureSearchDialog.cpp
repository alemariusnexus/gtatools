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

#include "TextureSearchDialog.h"
#include <gtatools-gui/config.h>
#include "../../ProfileManager.h"
#include "../../Profile.h"
#include <QString>
#include <QStringList>
#include <QInputDialog>
#include <QMessageBox>
#include "../../System.h"
#include <nxcommon/file/DefaultFileFinder.h>
#include "../../RegexFileFinder.h"
#include "../../gui/GUI.h"
#include <QSettings>



TextureSearchDialog::TextureSearchDialog(QWidget* parent, const QLinkedList<File>& rootFiles)
		: QDialog(parent), rootFiles(rootFiles), finder(NULL)
{
	ui.setupUi(this);
	connect(ui.searchButton, SIGNAL(clicked(bool)), this, SLOT(onSearch(bool)));
	connect(ui.cancelButton, SIGNAL(clicked(bool)), this, SLOT(onCancel(bool)));
}


void TextureSearchDialog::onCancel(bool checked)
{
	if (finder) {
		finder->interrupt();
	}

	close();
}


void TextureSearchDialog::onSearch(bool checked)
{
	ui.searchButton->setEnabled(false);

	FileFinder* texFinder = NULL;
	FileFinder* txdFinder = NULL;

	QString texName = ui.nameField->text();
	QString txdName = ui.txdField->text();

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

		QRegExp texRegex(texName, ui.caseBox->isChecked() ? Qt::CaseSensitive
				: Qt::CaseInsensitive, syntax);
		texFinder = new RegexFileFinder(texRegex, ui.exactBox->isChecked());

		if (!txdName.isEmpty()) {
			QRegExp txdRegex(txdName, ui.caseBox->isChecked() ? Qt::CaseSensitive
					: Qt::CaseInsensitive, syntax);
			txdFinder = new RegexFileFinder(txdRegex, ui.exactBox->isChecked());
		}
	} else {
		texFinder = new DefaultFileFinder(texName.toLocal8Bit().constData(),
				ui.caseBox->isChecked(), ui.exactBox->isChecked());

		if (!txdName.isEmpty()) {
			txdFinder = new DefaultFileFinder(txdName.toLocal8Bit().constData(),
					ui.caseBox->isChecked(), ui.exactBox->isChecked());
		}
	}

	finder = new TextureFileFinder(texFinder, txdFinder);

	File toBeOpened = GUI::getInstance()->findFile(finder, this);
	bool closeDialog = false;

	if (!toBeOpened.isNull()) {
		EntityOpenRequest req;
		req.setAttribute("file", QString(toBeOpened.getPath().toString().get()));
		const char* matchedTex = finder->getMatchedTexture(toBeOpened);
		req.setAttribute("texture", matchedTex);
		System::getInstance()->openEntity(req);
		closeDialog = true;
	}

	if (closeDialog) {
		close();
	} else {
		ui.searchButton->setEnabled(true);
	}
}

