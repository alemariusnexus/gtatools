/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "DefaultFileFinder.h"
#include "strutil.h"
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
