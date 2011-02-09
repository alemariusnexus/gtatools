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

#include "Task.h"
#include <deque>
#include <QtCore/QMutex>
#include <QtCore/QThread>
#include <QtCore/QTimer>
#include "gui/MainWindow.h"

using std::deque;


deque<Task*> taskQueue;
QMutex taskQueueMutex;




Task::Task(MainWindow* mw)
		: pb(mw->getProgressBar()), label(mw->getTaskLabel())
{
	connect(this, SIGNAL(pbSetMinimum(int)), pb, SLOT(setMinimum(int)));
	connect(this, SIGNAL(pbSetMaximum(int)), pb, SLOT(setMaximum(int)));
	connect(this, SIGNAL(pbSetValue(int)), pb, SLOT(setValue(int)));
	connect(this, SIGNAL(labelDisplay(const QString&)), label, SLOT(setText(const QString&)));
	connect(this, SIGNAL(labelClearMessage()), label, SLOT(clear()));
	connect(this, SIGNAL(labelAdjustSize()), mw, SLOT(taskLabelShouldAdjust()));
}


Task::~Task()
{
	taskQueueMutex.lock();

	bool current = isCurrent();

	taskQueue.erase(taskQueue.begin() + queuePos);

	if (current) {
		if (!taskQueue.empty()) {
			Task* task = taskQueue.back();
			task->display();
		} else {
			if (QThread::currentThread() == pb->thread()) {
				pb->setMinimum(0);
				pb->setMaximum(100);
				pb->setValue(0);
				label->setText(QString());
			} else {
				emit pbSetMinimum(0);
				emit pbSetMaximum(100);
				emit pbSetValue(0);
				emit labelDisplay(QString());
			}

			emit labelAdjustSize();
		}
	}

	taskQueueMutex.unlock();
}


void Task::start(int min, int max, const QString& message)
{
	currentValue = min;
	minimum = min;
	maximum = max;
	this->message = message;

	taskQueueMutex.lock();
	queuePos = taskQueue.size();
	taskQueue.push_back(this);
	display();
	taskQueueMutex.unlock();
}


void Task::update(int value)
{
	currentValue = value;

	taskQueueMutex.lock();

	if (isCurrent()) {
		display();
	}

	taskQueueMutex.unlock();
}


void Task::update()
{
	update(getValue()+1);
}


bool Task::isCurrent()
{
	return taskQueue.back() == this;
}


void Task::display()
{
	if (QThread::currentThread() == pb->thread()) {
		pb->setMinimum(minimum);
		pb->setMaximum(maximum);
		pb->setValue(currentValue);
		label->setText(message);
	} else {
		emit pbSetMinimum(minimum);
		emit pbSetMaximum(maximum);
		emit pbSetValue(currentValue);
		emit labelDisplay(message);
	}

	emit labelAdjustSize();
}


int Task::getValue()
{
	return currentValue;
}


int Task::getMinimum()
{
	return minimum;
}


int Task::getMaximum()
{
	return maximum;
}

