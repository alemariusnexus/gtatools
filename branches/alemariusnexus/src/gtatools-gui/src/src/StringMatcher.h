/*
 * StringMatcher.h
 *
 *  Created on: 15.07.2010
 *      Author: alemariusnexus
 */

#ifndef STRINGMATCHER_H_
#define STRINGMATCHER_H_

#include <qstring.h>
#include <qregexp.h>

#define STRING_MATCHER_REGEX (1 << 0)
#define STRING_MATCHER_CASEINSENSITIVE (1 << 1)
#define STRING_MATCHER_EXACTMATCH (1 << 2)



class StringMatcher {
public:
	typedef char flags;

public:
	StringMatcher(const QString& pattern, flags flags);
	bool matches(const QString& text);

private:
	QString* plainPattern;
	QRegExp* regexPattern;
	bool exactMatch;
	Qt::CaseSensitivity plainCaseSensitivity;
};

#endif /* STRINGMATCHER_H_ */
