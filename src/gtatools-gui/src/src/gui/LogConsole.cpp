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

#include "LogConsole.h"
#include <gtatools-gui/config.h>
#include "../System.h"
#include <QSettings>
#include <cassert>



LogConsole::LogConsole(QWidget* parent)
		: QWidget(parent)
{
	ui.setupUi(this);

	System* sys = System::getInstance();
	connect(sys, SIGNAL(entryLogged(const LogEntry&)), this, SLOT(entryLogged(const LogEntry&)));
	connect(ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonBoxClicked(QAbstractButton*)));
}


void LogConsole::entryLogged(const LogEntry& entry)
{
	QString richMsg = entry.getMessage();


	richMsg.replace("\t", "    ");
	richMsg.replace(" ", "&nbsp;");

	QString typeStr;
	QString colorStr;

	switch (entry.getType()) {
	case LogEntry::Debug:
		typeStr = tr("DEBUG");
		colorStr = "green";
		break;
	case LogEntry::Info:
		typeStr = tr("INFO");
		colorStr = "blue";
		break;
	case LogEntry::Warning:
		typeStr = tr("WARNING");
		colorStr = "orange";
		break;
	case LogEntry::Error:
		typeStr = tr("ERROR");
		colorStr = "red";
		break;
	default:
		assert(false);
	}

	const Exception* ex = entry.getException();

	if (ex) {
		richMsg.append(QString(tr(" (caused by %1)")).arg(ex->getName().get()));
	}

	ui.logTextEdit->append(QString("<font color=\"%1\"><b>%2:</b> %3</font>").arg(colorStr).arg(typeStr)
			.arg(richMsg));
}


void LogConsole::buttonBoxClicked(QAbstractButton* button)
{
	if (ui.buttonBox->standardButton(button) == QDialogButtonBox::Reset) {
		ui.logTextEdit->clear();
	}
}
