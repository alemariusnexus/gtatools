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
		task->update((int) (((float)filesDone / (float) numFiles)*100.0f));
	} else {
		if (filesDone%(numFiles / 100) == 0) {
			task->update(task->getValue()+1);
		}
	}

	qApp->processEvents();

	return matches;
}
