/*
 * FileFinder.h
 *
 *  Created on: 13.10.2010
 *      Author: alemariusnexus
 */

#ifndef FILEFINDER_H_
#define FILEFINDER_H_

#include "File.h"


class FileFinder {
public:
	virtual bool matches(const File& file) = 0;
	virtual bool isInterrupted() { return interrupted; }
	virtual void interrupt() { interrupted = true; }

protected:
	FileFinder() : interrupted(false) {}

private:
	bool interrupted;
};

#endif /* FILEFINDER_H_ */
