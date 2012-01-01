/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#ifndef TASKFILEFINDER_H_
#define TASKFILEFINDER_H_

#include <gtaformats/util/FileFinder.h>
#include "Task.h"



class TaskFileFinder : public FileFinder {
public:
	TaskFileFinder(FileFinder* backend, const QString& message, int numFiles = -1,
			bool deleteBackend = false);
	~TaskFileFinder();
	virtual bool matches(const File& file);
	virtual void interrupt() { backend->interrupt(); }
	virtual bool isInterrupted() { return backend->isInterrupted(); }

private:
	FileFinder* backend;
	int numFiles;
	int filesDone;
	bool deleteBackend;
	Task* task;
};

#endif /* TASKFILEFINDER_H_ */
