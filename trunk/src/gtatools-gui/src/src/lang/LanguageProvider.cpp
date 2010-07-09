/*
	Copyright 2010 David Lerch

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
