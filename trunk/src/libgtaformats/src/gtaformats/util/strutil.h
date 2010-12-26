/*
 * strutil.h
 *
 *  Created on: 23.12.2010
 *      Author: alemariusnexus
 */

#ifndef STRUTIL_H_
#define STRUTIL_H_

#include "../config.h"


/**	\brief Converts src to lower case.
 *
 * 	src is assumed to be ASCII.
 *
 * 	@param dest The buffer to write the lower case characters to.
 *	@param src The mixed case characters.
 */
void strtolower(char* dest, const char* src);


/**	\brief Trims chr from the right end of str.
 *
 * 	@param str The string to trim.
 * 	@param chr The cahracter to remove from the end of str.
 */
void rtrim(char* str, char chr);

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


#endif /* STRUTIL_H_ */
