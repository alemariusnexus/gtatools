/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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
 */

#include "util.h"
#include <cctype>
#include <cstring>

#ifdef linux
#include <sys/time.h>
#else
#include <windows.h>
#endif




void strtolower(char* dest, const char* src)
{
	int i;
	for (i = 0 ; src[i] != '\0' ; i++) {
		dest[i] = tolower(src[i]);
	}

	dest[i] = '\0';
}


void rtrim(char* str, char chr)
{
	int len = strlen(str);

	if (len == 0) {
		return;
	}

	str += len-1;
	while (*str == chr) *str-- = '\0';
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


uint64_t GetTickcount()
{
#ifdef linux
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec*1000 + tv.tv_usec/1000;
#else
	return GetTickCount();
#endif
}
