/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#ifndef TASK_H_
#define TASK_H_

#include <QtCore/QObject>
#include <QtGui/QProgressBar>
#include <QtGui/QLabel>
#include <cstdio>

class MainWindow;


class Task : public QObject {
	Q_OBJECT

public:
	Task(MainWindow* mw);
	~Task();
	void start(int min, int max, const QString& message);
	void start(const QString& message) { start(0, 0, message); update(-1); };
	void update(int value);
	void update();
	bool isCurrent();
	int getValue();
	int getMinimum();
	int getMaximum();

protected:
	void display();

signals:
	void pbSetMinimum(int min);
	void pbSetMaximum(int max);
	void pbSetValue(int value);
	void labelDisplay(const QString& msg);
	void labelClearMessage();
	void labelAdjustSize();

private:
	QProgressBar* pb;
	QLabel* label;
	QString message;
	int minimum;
	int maximum;
	int currentValue;
	int queuePos;
};

#endif /* TASK_H_ */
