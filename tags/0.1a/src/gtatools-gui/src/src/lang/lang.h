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

#ifndef LANG_H_
#define LANG_H_

#include <cstdarg>
#include <cstdio>
#include <wx/wx.h>
#include <wx/regex.h>
#include <map>
#include <utility>
#include "LanguageProvider.h"
#include "EnglishLanguageProvider.h"
#include "GermanLanguageProvider.h"

using std::map;
using std::pair;



/**	\brief The language which is used by the program.
 */
const GermanLanguageProvider defaultLanguage;



/**	\brief A shortcut to get a formatted language entry by it's ID.
 *
 * 	The call is just forwarded to defaultLanguage.getStringFormattedV().
 *
 *	@param id The language ID.
 *	@return The formatted language string.
 *	\see LanguageProvider
 */
inline wxString LangGet(const char* id, ...)
{
	va_list list;
	va_start(list, id);
	wxString str = defaultLanguage.getStringFormattedV(id, list);
	va_end(list);
	return str;
}


#endif /* LANG_H_ */
