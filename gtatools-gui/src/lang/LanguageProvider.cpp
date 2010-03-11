/*
 * LanguageProvider.cpp
 *
 *  Created on: 27.02.2010
 *      Author: alemariusnexus
 */

#include "LanguageProvider.h"
#include <wx/regex.h>
#include <iostream>

using std::cerr;
using std::endl;


wxString LanguageProvider::getString(const char* idStr) const
{
	EntryMap::const_iterator it = entryMap.find(idStr);

	if (it == entryMap.end()) {
		cerr << "WARNING: Language entry with ID " << idStr << " was not found!" << endl;
		return wxT("ID_NOT_FOUND");
	}

	return resolveBackreferences(it->second);
}


wxString LanguageProvider::getStringFormattedV(const char* idStr, va_list list) const
{
	wxString value = getString(idStr);
	wxString str = wxString::FormatV(value, list);
	return str;
}


wxString LanguageProvider::getStringFormatted(const char* idStr, ...) const
{
	va_list list;
	va_start(list, idStr);
	wxString str = getStringFormattedV(idStr, list);
	va_end(list);
	return str;
}


void LanguageProvider::addEntry(const char* id, const wxChar* value)
{
	entryMap.insert(pair<const char*, const wxChar*>(id, value));
}


wxString LanguageProvider::resolveBackreferences(const wxString& str) const
{
	wxString result;
	wxRegEx regex(wxT("\\$\\{([a-zA-Z_][a-zA-Z0-9_]*)\\}"));
	wxString subject = str;
	size_t start, len;

	while (regex.Matches(subject)) {
		regex.GetMatch(&start, &len, 0);
		wxString idStr = subject.substr(start+2, len-3);
		result << subject.substr(0, start);
		result << getString(idStr.mb_str());
		subject = subject.substr(start+len);
	}

	result << subject;

	return result;
	//return wxString(wxT("Test %s"));
}
