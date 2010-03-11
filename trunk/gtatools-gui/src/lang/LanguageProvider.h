/*
 * LanguageProvider.h
 *
 *  Created on: 27.02.2010
 *      Author: alemariusnexus
 */

#ifndef LANGUAGEPROVIDER_H_
#define LANGUAGEPROVIDER_H_

#include <wx/string.h>
#include <map>
#include <utility>
#include <cstring>

using std::map;
using std::pair;



/**	\brief A function object used internally as comparator for LanguageProvider's EntryMap.
 */
class StringComparator {
public:
	bool operator()(const char* c1, const char* c2) const {
		return strcmp(c1, c2) > 0;
	}
};


/**	\brief Represents a target language for program internationalization.
 *
 * 	Usually, there's only one, static instance of a subclass of LanguageProvider which is the language
 * 	used by the program. The LanguageProvider used by the program can be set in lang.h.
 */
class LanguageProvider
{
private:
	typedef map<const char*, const wxChar*, StringComparator> EntryMap;

public:
	/**	\brief Returns a string in the provider's language with the given ID.
	 *
	 * 	These strings can contain backreferences of the form ${id}, where id is the ID of another language
	 * 	entry. These backreferences will be replaced.
	 *
	 *	@param idStr The ID of the entry to retrieve.
	 *	@return The language string.
	 */
	wxString getString(const char* idStr) const;

	/**	\brief Returns a formatted string in the provider's language with the given ID, using a va_list.
	 *
	 * 	This is the same as getStringFormatted(), but it takes the va_list directly.
	 *
	 *	@param idStr The ID of the entry to retrieve.
	 *	@param list The va_list to use for formatting.
	 *	@return The formatted language string.
	 *	@see getStringFormatted()
	 */
	wxString getStringFormattedV(const char* idStr, va_list list) const;

	/**	\brief Returns a formatted string in the provider's language with the given ID.
	 *
	 * 	This method does the same as getString(), but replaces format operators in the language string using
	 * 	wxString::Format().
	 *
	 * @param idStr The ID of the entry to retrieve.
	 * @return The formatted language string.
	 */
	wxString getStringFormatted(const char* idStr, ...) const;

protected:
	/**	\brief This can be used by subclasses to register a language entry.
	 *
	 * 	The language ID is the ID with which is referred to the value.
	 *
	 *	@param id The language ID.
	 *	@param value The string value.
	 */
	void addEntry(const char* id, const wxChar* value);

private:
	/**	\brief Resolves and replaces backreferences in the string.
	 *
	 * 	Backreferences of the form ${id}, where id is a language ID, are replaced by the entries' values.
	 * 	This is done recursively for the referenced entries.
	 *
	 *	@param str The string containing backreferences.
	 *	@return The string with resolved backreferences.
	 */
	wxString resolveBackreferences(const wxString& str) const;

private:
	EntryMap entryMap;
};

#endif /* LANGUAGEPROVIDER_H_ */
