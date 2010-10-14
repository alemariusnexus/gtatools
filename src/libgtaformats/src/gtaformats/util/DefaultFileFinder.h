/*
 * DefaultFileFinder.h
 *
 *  Created on: 13.10.2010
 *      Author: alemariusnexus
 */

#ifndef DEFAULTFILEFINDER_H_
#define DEFAULTFILEFINDER_H_

#include "FileFinder.h"



class DefaultFileFinder : public FileFinder {
public:
	DefaultFileFinder(const char* pattern, bool caseSensitive = true, bool exactMatch = true);
	void setExactMatch(bool em) { exactMatch = em; }
	bool isCaseSensitive() const { return caseSensitive; }
	bool isExactMatch() const { return exactMatch; }
	virtual bool matches(const File& file);

private:
	char* pattern;
	bool caseSensitive;
	bool exactMatch;
};

#endif /* DEFAULTFILEFINDER_H_ */
