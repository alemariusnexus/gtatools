/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef STRUTIL_H_
#define STRUTIL_H_

#include <gtaformats/config.h>
#include <locale>
#include <cstring>
#include <cstdio>

using std::locale;
using std::collate;
using std::use_facet;


typedef long hash_t;



/**	\brief Converts src to lower case.
 *
 * 	src is assumed to be ASCII.
 *
 * 	@param dest The buffer to write the lower case characters to.
 *	@param src The mixed case characters.
 */
void strtolower(char* dest, const char* src, size_t len);
void strtoupper(char* dest, const char* src, size_t len);

inline void strtolower(char* dest, const char* src)
{
	size_t len = strlen(src);
	strtolower(dest, src, len);
	dest[len] = '\0';
}
inline void strtoupper(char* dest, const char* src)
{
	size_t len = strlen(src);
	strtoupper(dest, src, len);
	dest[len] = '\0';
}



void rtrim(char* str, const char* chrs);

/**	\brief Trims chr from the right end of str.
 *
 * 	@param str The string to trim.
 * 	@param chr The cahracter to remove from the end of str.
 */
inline void rtrim(char* str, char chr) { char b[2]; b[0] = chr; b[1] = '\0'; rtrim(str, b); }

const char* ltrim(const char* str, char chr);

/**	\brief Breaks a string into multiple lines using word wrapping.
 *
 * 	This function will try to wrap words by breaking at whitespace positions. If a word is longer than the
 * 	number of chars per line, it will however be broken on the last possible position in line.
 * 	Note that only plain space characters are used as word boundaries. Tabulators, Newlines and other
 * 	whitespace will break the wrapping.
 * 	The added newlines will always be \n only.
 *
 * 	@param src The text to be wrapped.
 * 	@param cpl Maximum number of characters per line.
 */
char* wrapText(const char* src, int cpl);

/**	\brief Indents a text.
 *
 * 	This inserts the indentation string after every newline in the string. It is assumed that the input
 * 	string uses only \n as line terminator.
 * 	Note that the indentation string is NOT added to the beginning of the string.
 */
char* indent(const char* src, const char* indStr);


bool WildcardMatch(const char* pattern, const char* text);



inline hash_t Hash(const char* str, size_t len)
{
	return use_facet< collate<char> >(locale()).hash(str, str+len);
}


inline hash_t Hash(const char* str)
{
	return Hash(str, strlen(str));
}


hash_t LowerHash(const char* str, size_t len);

inline hash_t LowerHash(const char* str)
{
	return LowerHash(str, strlen(str));
}


void FormatByteSize(char* str, uint64_t size, int precision = 3);


#endif /* STRUTIL_H_ */
