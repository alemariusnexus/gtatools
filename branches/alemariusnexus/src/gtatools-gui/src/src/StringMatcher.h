/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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
