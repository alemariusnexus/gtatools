/*
 * DefaultFileFinder.cpp
 *
 *  Created on: 13.10.2010
 *      Author: alemariusnexus
 */

#include "DefaultFileFinder.h"
#include "util.h"
#include <cstring>



DefaultFileFinder::DefaultFileFinder(const char* pattern, bool caseSensitive, bool exactMatch)
		: pattern(new char[strlen(pattern)+1]), caseSensitive(caseSensitive), exactMatch(exactMatch)
{
	if (caseSensitive) {
		strcpy(this->pattern, pattern);
	} else {
		strtolower(this->pattern, pattern);
	}
}


bool DefaultFileFinder::matches(const File& file)
{
	const char* fname = file.getPath()->getFileName();

	if (caseSensitive) {
		if (exactMatch) {
			return strcmp(fname, pattern) == 0;
		} else {
			return strstr(fname, pattern) != NULL;
		}
	} else {
		char* lfname = new char[strlen(fname)+1];
		strtolower(lfname, fname);
		bool res;

		if (exactMatch) {
			res = strcmp(lfname, pattern) == 0;
		} else {
			res = strstr(lfname, pattern) != NULL;
		}

		delete[] lfname;
		return res;
	}
}
