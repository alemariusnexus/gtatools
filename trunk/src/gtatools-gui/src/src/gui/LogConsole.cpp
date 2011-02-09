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
 */

#include "LogConsole.h"
#include "../config.h"
#include "../System.h"
#include <QtCore/QSettings>



LogConsole::LogConsole(QWidget* parent)
		: QWidget(parent)
{
	ui.setupUi(this);

	System* sys = System::getInstance();
	connect(sys, SIGNAL(errorLogged(const QString&)), this, SLOT(errorLogged(const QString&)));
	connect(ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonBoxClicked(QAbstractButton*)));
}


void LogConsole::errorLogged(const QString& errmsg)
{
	QString richMsg = errmsg;
	richMsg.replace("\t", "    ");
	richMsg.replace(" ", "&nbsp;");

	ui.logTextEdit->append(QString("<font color=\"red\"><b>%1:</b> %2</font>").arg(tr("ERROR")).arg(richMsg));
}


void LogConsole::buttonBoxClicked(QAbstractButton* button)
{
	if (ui.buttonBox->standardButton(button) == QDialogButtonBox::Reset) {
		ui.logTextEdit->clear();
	}
}
