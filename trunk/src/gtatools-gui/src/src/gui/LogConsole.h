/*
 * LogTextEdit.h
 *
 *  Created on: 11.10.2010
 *      Author: alemariusnexus
 */

#ifndef LOGCONSOLE_H_
#define LOGCONSOLE_H_

#include <QtGui/QWidget>
#include <ui_LogConsole.h>
#include <QtCore/QString>



class LogConsole : public QWidget {
	Q_OBJECT

public:
	LogConsole();

private slots:
	void errorLogged(const QString& errmsg);
	void buttonBoxClicked(QAbstractButton* button);
	void autoShowBoxChanged(int state);

private:
	Ui_LogConsole ui;
};

#endif /* LOGCONSOLE_H_ */
