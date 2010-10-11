/*
 * LogTextEdit.cpp
 *
 *  Created on: 11.10.2010
 *      Author: alemariusnexus
 */

#include <config.h>
#include "LogConsole.h"
#include "../System.h"
#include <QtCore/QSettings>



LogConsole::LogConsole()
		: QWidget()
{
	ui.setupUi(this);

	System* sys = System::getInstance();
	connect(sys, SIGNAL(errorLogged(const QString&)), this, SLOT(errorLogged(const QString&)));
	connect(ui.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonBoxClicked(QAbstractButton*)));
	connect(ui.autoShowBox, SIGNAL(stateChanged(int)), this, SLOT(autoShowBoxChanged(int)));

	QSettings settings(CONFIG_FILE, QSettings::IniFormat);
	ui.autoShowBox->setChecked(settings.value("main/auto_show_error_log_console", true).toBool());
}


void LogConsole::errorLogged(const QString& errmsg)
{
	QString richMsg = errmsg;
	richMsg.replace("\t", "    ");
	richMsg.replace(" ", "&nbsp;");

	ui.logTextEdit->append(QString("<font color=\"red\"><b>%1:</b> %2</font>").arg(tr("ERROR")).arg(richMsg));

	if (!isVisible()) {
		QSettings settings(CONFIG_FILE, QSettings::IniFormat);
		if (settings.value("gui/auto_show_error_log_console", true).toBool()) {
			show();
		}
	}
}


void LogConsole::buttonBoxClicked(QAbstractButton* button)
{
	if (ui.buttonBox->standardButton(button) == QDialogButtonBox::Reset) {
		ui.logTextEdit->clear();
	}
}


void LogConsole::autoShowBoxChanged(int state)
{
	QSettings settings(CONFIG_FILE, QSettings::IniFormat);
	settings.setValue("gui/auto_show_error_log_console", ui.autoShowBox->isChecked());
}
