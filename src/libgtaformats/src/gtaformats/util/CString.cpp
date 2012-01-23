/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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



void CString::copy()
{
	size_t len = length();
	shared_array<char> cpy(new char[len+1]);
	strcpy(cpy.get(), cstr.get());
	cstr = cpy;
}


CString& CString::ltrim(char c)
{
	char* s = cstr.get();
	char* os = s;
	int len = strlen(s);

	while (s-os != len  &&  *s == c) s++;

	char* nStr = new char[len - (s-os)+1];
	strcpy(nStr, s);

	cstr = shared_array<char>(nStr);

	return *this;
}


void CString::append(const CString& other)
{
	size_t len = length() + other.length();
	shared_array<char> cpy(new char[len+1]);
	strcpy(cpy.get(), cstr.get());
	strcat(cpy.get(), other.cstr.get());
	cstr = cpy;
}
