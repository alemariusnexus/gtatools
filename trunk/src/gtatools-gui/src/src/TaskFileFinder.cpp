/*
 * TaskFileFinder.cpp
 *
 *  Created on: 14.10.2010
 *      Author: alemariusnexus
 */

#include "TaskFileFinder.h"
#include "System.h"




TaskFileFinder::TaskFileFinder(FileFinder* backend, const QString& message, int numFiles, bool deleteBackend)
		: backend(backend), numFiles(numFiles), filesDone(0), deleteBackend(deleteBackend)
{
	System* sys = System::getInstance();
	task = sys->createTask();

	if (numFiles != -1) {
		task->start(0, 100, message);
	} else {
		task->start(0, 0, message);
	}
}


TaskFileFinder::~TaskFileFinder()
{
	delete task;

	if (deleteBackend) {
		delete backend;
	}
}


bool TaskFileFinder::matches(const File& file)
{
	bool matches = backend->matches(file);

	filesDone++;

	if (numFiles < 100) {
		System* sys = System::getInstance();
		task->update((int) (((float)filesDone / (float) numFiles)*100.0f));
	} else {
		if (filesDone%(numFiles / 100) == 0) {
			System* sys = System::getInstance();
			task->update(task->getValue()+1);
		}
	}

	qApp->processEvents();

	return matches;
}
