/*
 * lang.h
 *
 *  Created on: 25.01.2010
 *      Author: alemariusnexus
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
