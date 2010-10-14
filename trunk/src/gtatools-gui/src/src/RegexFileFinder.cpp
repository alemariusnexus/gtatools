/*
 * RegexFileFinder.cpp
 *
 *  Created on: 13.10.2010
 *      Author: alemariusnexus
 */

#include "RegexFileFinder.h"



RegexFileFinder::RegexFileFinder(const QRegExp& regex, bool exactMatch)
		: regex(regex), exactMatch(exactMatch)
{
}


bool RegexFileFinder::matches(const File& file)
{
	QString fname = file.getPath()->getFileName();

	if (exactMatch) {
		return regex.exactMatch(fname);
	} else {
		return regex.indexIn(fname) != -1;
	}
}
