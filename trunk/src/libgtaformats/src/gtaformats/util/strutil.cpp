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

#include "strutil.h"
#include <string>
#include <cstring>
#include <cstdio>

using std::string;



void strtolower(char* dest, const char* src, size_t len)
{
	size_t i;
	for (i = 0 ; i < len  &&  src[i] != '\0' ; i++) {
		dest[i] = tolower(src[i]);
	}
}


void strtoupper(char* dest, const char* src, size_t len)
{
	size_t i;
	for (i = 0 ; i < len  &&  src[i] != '\0' ; i++) {
		dest[i] = toupper(src[i]);
	}
}


void rtrim(char* str, const char* chrs)
{
	int len = strlen(str);

	if (len == 0) {
		return;
	}

	char* strBeg = str;
	str += len-1;
	//while (str >= strBeg  &&  *str == chr) *str-- = '\0';
	while (str >= strBeg  &&  strchr(chrs, *str)  !=  NULL) *str-- = '\0';
}


const char* ltrim(const char* str, char chr)
{
	int len = strlen(str);
	while (str != str+len  &&  *str == chr) str++;
	return str;
}


char* wrapText(const char* src, int cpl)
{
	string out;
	const char* lastWordStart = src;
	int numCharsInLine = 0;

	do {
		if (src-lastWordStart > cpl) {
			// The currently parsed word is yet so long that it doesn't fit into a single line at all. This
			// means it will have to be split in the middle, which we do here. We put as many characters of
			// the word as possible into the current line.

			int charsLeft = cpl-numCharsInLine;
			out.append(lastWordStart, charsLeft);
			out += '\n';

			// Reposition src to point to the next character of the long word, which didn't fit on this line.
			src = lastWordStart+charsLeft;

			lastWordStart = src; // Pretend the rest is a new word
			numCharsInLine = 0;
			continue;
		}

		if (*src == '\0'  ||  *src == ' '  ||  *src == '\n') {
			int wordLen = src-lastWordStart; // Without space
			int charsLeft = cpl-numCharsInLine;

			if (charsLeft < wordLen) {
				// Not enough space for the word -> Create a newline
				out[out.length()-1] = '\n'; // A space was there before

				// For simplicity, we will just parse the word again. -1 is because src is incremented at
				// loop end.
				src = lastWordStart-1;

				numCharsInLine = 0;
			} else {
				out.append(lastWordStart, wordLen);

				if (charsLeft != wordLen  &&  *src == ' ') {
					// *src is a space, and there is enough space left in the line to append it.
					out += ' ';
					numCharsInLine += wordLen+1;
				} else {
					if (*src == ' ') {
						// *src is a space, but there's no space left for it. We will explicitly create a
						// newline here, because the algorithm assumes there's always a space after the last
						// inserted word.
						// We don't create this newline if this is the string end ('\0'). Also, if *src is
						// a hard break ('\n'), we use that as our newline.
						out += '\n';
					}
					numCharsInLine = 0;
				}

				if (*src == '\n') {
					// A hard break always breaks the current line.
					out += '\n';
					numCharsInLine = 0;
				}

				// Next word
				lastWordStart = src+1;
			}
		}
	} while (*src++ != '\0');

	char* ret = new char[out.length()+1];
	strcpy(ret, out.c_str());

	return ret;
}


char* indent(const char* src, const char* indStr)
{
	const char* srcStart = src;
	int srcLen = strlen(src);
	int indStrLen = strlen(indStr);

	int numLines = 1;

	while (*src != '\0') {
		if (*src++ == '\n') {
			numLines++;
		}
	}

	src = srcStart;
	char* dest = new char[srcLen + (numLines-1)*indStrLen + 1];
	char* destStart = dest;

	do {
		dest[0] = *src;
		dest++;

		if (*src == '\n') {
			strcpy(dest, indStr);
			dest += indStrLen;
		}

	} while (*src++ != '\0');

	return destStart;
}


bool WildcardMatch(const char* pattern, const char* text)
{
	bool escaped = false;
	while (*text != '\0') {
		if (escaped) {
			if (*text != *pattern) {
				return false;
			}
			escaped = false;
		} else {
			switch (*pattern) {
			case '?':
				break;
			case '*':
				do {
					pattern++;
				} while(*pattern == '*');
				if (*pattern == '\0') {
					return true;
				}
				while (*text != '\0') {
					if (WildcardMatch(pattern, text)) {
						return true;
					}
					text++;
				}
				return false;
			case '\\':
				escaped = true;
				text--;
				break;
			default:
				if (*pattern != *text) {
					return false;
				}
				break;
			}
		}
		text++;
		pattern++;
	}
	while (*pattern == '*') pattern++;
	return *pattern == '\0';
}


hash_t LowerHash(const char* str, size_t len)
{
	char* lStr = new char[len];
	strtolower(lStr, str, len);
	hash_t hash = Hash(lStr, len);
	delete[] lStr;
	return hash;
}


void FormatByteSize(char* str, uint64_t size, int precision)
{
	if (size > 1000000000) {
		sprintf(str, "%.*fGB", precision, size/1000000000.0f);
	} else if (size > 1000000) {
		sprintf(str, "%.*fMB", precision, size/1000000.0f);
	} else if (size > 1000) {
		sprintf(str, "%.*fkB", precision, size/1000.0f);
	} else {
		sprintf(str, "%llu", (long long unsigned int) size);
	}
}
