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

#ifndef CSTRING_H_
#define CSTRING_H_

#include <boost/shared_array.hpp>
#include <cstring>
#include "strutil.h"

using boost::shared_array;



class CString {
public:
	static CString from(char* s)
			{ CString c; c.cstr = shared_array<char>(s); return c; }

public:
	CString(const char* cstr, size_t len)
	{
		if (cstr) {
			this->cstr = shared_array<char>(new char[len+1]);
			this->cstr[len] = '\0';
			strncpy(this->cstr.get(), cstr, len);
		} else {
			this->cstr = shared_array<char>(NULL);
		}
	}
	CString(const char* cstr)
	{
		if (cstr) {
			this->cstr = shared_array<char>(new char[strlen(cstr)+1]);
			strcpy(this->cstr.get(), cstr);
		} else {
			this->cstr = shared_array<char>(NULL);
		}
	}
	CString(const CString& other) : cstr(other.cstr) {}
	CString() : cstr(NULL) {}
	size_t length() const { return strlen(cstr.get()); }
	CString& lower() { ensureUniqueness(); strtolower(cstr.get(), cstr.get()); return *this; }
	CString& upper() { ensureUniqueness(); strtoupper(cstr.get(), cstr.get()); return *this; }
	CString& ltrim(char c = ' ');
	CString& rtrim(char c = ' ') { ensureUniqueness(); ::rtrim(cstr.get(), c); return *this; }
	CString& trim(char c = ' ') { rtrim(c); ltrim(c); return *this; }
	const char* get() const { return cstr.get(); }
	void append(const CString& other);

	bool operator<(const CString& other) const { return strcmp(cstr.get(), other.cstr.get()) < 0; }
	bool operator>(const CString& other) const { return strcmp(cstr.get(), other.cstr.get()) > 0; }
	bool operator<=(const CString& other) const { return !(*this > other); }
	bool operator>=(const CString& other) const { return !(*this < other); }
	bool operator==(const CString& other) const
	{
		return cstr.get() == 0  ||  other.cstr.get() == 0
				? cstr.get() == other.cstr.get()
				: strcmp(cstr.get(), other.cstr.get()) == 0;
	}
	bool operator!=(const CString& other) const { return !(*this == other); }
	char& operator[](size_t idx) { ensureUniqueness(); return cstr[idx]; }
	const char& operator[](size_t idx) const { return cstr[idx]; }
	CString& operator=(const CString& other) { cstr = other.cstr; return *this; }

private:
	void ensureUniqueness() { if (!cstr.unique()) copy(); }
	void copy();

private:
	shared_array<char> cstr;
};

#endif /* CSTRING_H_ */
