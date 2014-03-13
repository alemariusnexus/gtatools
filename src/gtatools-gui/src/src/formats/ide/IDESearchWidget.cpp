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

#include "IDESearchWidget.h"
#include "IDEFileFinder.h"
#include "../../gui/GUI.h"
#include "../../System.h"
#include "../../RegexFileFinder.h"
#include <nxcommon/file/DefaultFileFinder.h>
#include <QtCore/QSettings>




IDESearchWidget::IDESearchWidget(QWidget* parent)
		: QDialog(parent), finder(NULL)
{
	ui.setupUi(this);

	connect(ui.cancelButton, SIGNAL(clicked(bool)), this, SLOT(onCancel(bool)));
	connect(ui.searchButton, SIGNAL(clicked(bool)), this, SLOT(onSearch(bool)));
}


void IDESearchWidget::onCancel(bool checked)
{
	if (finder) {
		finder->interrupt();
	}

	close();
}


void IDESearchWidget::onSearch(bool checked)
{
	ui.searchButton->setEnabled(false);

	int32_t id = ui.idSpinner->value();
	QString meshName = ui.meshNameField->text();
	QString txdName = ui.txdNameField->text();
	bool exactMatch = ui.exactBox->isChecked();
	bool regex = ui.regexBox->isChecked();
	bool caseSensitive = ui.caseBox->isChecked();
	idetype_t types = 0;

	if (ui.animBox->isChecked())
		types |= IDETypeAnimation;
	if (ui.pedBox->isChecked())
		types |= IDETypePedestrian;
	if (ui.sobjBox->isChecked())
		types |= IDETypeStaticObject;
	if (ui.tobjBox->isChecked())
		types |= IDETypeTimedObject;
	if (ui.weaponBox->isChecked())
		types |= IDETypeWeapon;

	FileFinder* meshFinder = NULL;
	FileFinder* txdFinder = NULL;

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

	if (!meshName.isEmpty()) {
		if (regex)
			meshFinder = new RegexFileFinder(QRegExp(meshName,
					caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive, syntax), exactMatch);
		else
			meshFinder = new DefaultFileFinder(meshName.toLocal8Bit().constData(), caseSensitive,
					exactMatch);
	}

	if (!txdName.isEmpty()) {
		if (regex)
			txdFinder = new RegexFileFinder(QRegExp(txdName,
					caseSensitive ? Qt::CaseSensitive : Qt::CaseInsensitive, syntax), exactMatch);
		else
			txdFinder = new DefaultFileFinder(txdName.toLocal8Bit().constData(), caseSensitive,
					exactMatch);
	}

	finder = new IDEFileFinder(id, types, meshFinder, txdFinder);
	File toBeOpened = GUI::getInstance()->findFile(finder, this);

	if (meshFinder)
		delete meshFinder;
	if (txdFinder)
		delete txdFinder;

	if (!toBeOpened.isNull()) {
		int line = finder->getMatchedLine(toBeOpened);
		EntityOpenRequest req;
		req.setAttribute("file", QString(toBeOpened.getPath().toString().get()));
		req.setAttribute("line", line);
		System::getInstance()->openEntity(req);
	}

	delete finder;
	finder = NULL;

	if (!toBeOpened.isNull()) {
		close();
	} else {
		ui.searchButton->setEnabled(true);
	}
}
