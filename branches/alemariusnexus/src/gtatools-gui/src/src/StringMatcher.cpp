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

#include "StringMatcher.h"
#include <qsettings.h>
#include <config.h>


StringMatcher::StringMatcher(const QString& pattern, flags flags)
		: plainPattern(NULL), regexPattern(NULL)
{
	QSettings settings(CONFIG_FILE, QSettings::IniFormat);

	if ((flags & STRING_MATCHER_REGEX) != 0) {
		QRegExp::PatternSyntax syntax;

		QString cfgSyntax = settings.value("main/regex_syntax", "wildcard").toString();

		if (cfgSyntax == "wildcard") {
			syntax = QRegExp::WildcardUnix;
		} else if (cfgSyntax == "full") {
			syntax = QRegExp::RegExp;
		} else {
			syntax = QRegExp::WildcardUnix;
		}

		regexPattern = new QRegExp(pattern, ((flags & STRING_MATCHER_CASEINSENSITIVE) != 0)
				? Qt::CaseInsensitive : Qt::CaseSensitive, syntax);
	} else {
		plainPattern = new QString(pattern);
		plainCaseSensitivity = ((flags & STRING_MATCHER_CASEINSENSITIVE) != 0)
				? Qt::CaseInsensitive : Qt::CaseSensitive;
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

