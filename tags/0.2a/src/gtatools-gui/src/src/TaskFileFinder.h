/*
 * TaskFileFinder.h
 *
 *  Created on: 14.10.2010
 *      Author: alemariusnexus
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
