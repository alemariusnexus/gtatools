/*
 * StringMatcher.cpp
 *
 *  Created on: 15.07.2010
 *      Author: alemariusnexus
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

