/*
 * Task.h
 *
 *  Created on: 14.08.2010
 *      Author: alemariusnexus
 */

#ifndef TASK_H_
#define TASK_H_

#include <QObject>
#include <QProgressBar>
#include <QLabel>
//#include "gui/MainWindow.h"

class MainWindow;


class Task : public QObject {
	Q_OBJECT

public:
	Task(MainWindow* mw);
	~Task();
	void start(int min, int max, const QString& message);
	void start(const QString& message) { start(0, 0, message); };
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
