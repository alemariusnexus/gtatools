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

#include "CString.h"



void CString::grow(size_t minSize)
{
	if (minSize == 0) {
		bufSize = 64;
		realloc(bufSize);
	} else {
		if (minSize > bufSize) {
			bufSize = minSize*2;
			realloc(bufSize);
		} else {
			ensureUniqueness();
		}
	}
}


void CString::realloc(size_t size)
{
	bufSize = size;
	shared_array<char> cpy(new char[bufSize]);
	if (cstr.get())
		strcpy(cpy.get(), cstr.get());
	else
		cpy[0] = '\0';
	cstr = cpy;
}


CString& CString::ltrim(const char* chars)
{
	char* s = cstr.get();

	if (!s)
		return *this;

	char* os = s;
	int len = strlen(s);

	//while (s-os != len  &&  *s == c) s++;
	while (s-os != len  &&  strchr(chars, *s)  !=  NULL) s++;

	char* nStr = new char[bufSize];
	strcpy(nStr, s);

	cstr = shared_array<char>(nStr);

	return *this;
}


CString& CString::append(const CString& other)
{
	if (other.get()) {
		grow(length() + other.length() + 1);
		strcat(cstr.get(), other.cstr.get());
	}

	return *this;
}


CString CString::substr(size_t begin, size_t len) const
{
	char* sub = new char[len+1];
	strncpy(sub, cstr.get() + begin, len);
	sub[len] = '\0';
	return CString::from(sub);
}
