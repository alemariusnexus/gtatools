/*
 * StringMatcher.cpp
 *
 *  Created on: 15.07.2010
 *      Author: alemariusnexus
 */

#include "StringMatcher.h"


StringMatcher::StringMatcher(const QString& pattern, flags flags)
		: plainPattern(NULL), regexPattern(NULL)
{
	if ((flags & STRING_MATCHER_REGEX) != 0) {
		regexPattern = new QRegExp(pattern, ((flags & STRING_MATCHER_CASEINSENSITIVE) != 0)
				? Qt::CaseInsensitive : Qt::CaseSensitive, QRegExp::WildcardUnix);
	} else {
		plainPattern = new QString(pattern);
	}

	exactMatch = ((flags & STRING_MATCHER_EXACTMATCH) != 0);
}


bool StringMatcher::matches(const QString& text)
{
	if (plainPattern) {
		if (exactMatch) {
			return plainPattern->compare(text, plainCaseSensitivity) == 0;
		} else {
			return text.contains(*plainPattern, plainCaseSensitivity);
		}
	} else {
		if (exactMatch) {
			return regexPattern->exactMatch(text);
		} else {
			return regexPattern->indexIn(text) != -1;
		}
	}
}

