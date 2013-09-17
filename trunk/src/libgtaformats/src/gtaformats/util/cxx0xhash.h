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

#ifndef CXX0XHASH_H_
#define CXX0XHASH_H_

#include <gtaformats/config.h>
#include <string>
#include <boost/shared_array.hpp>
#include "CString.h"

using std::size_t;




#ifdef CXX0X_AVAILABLE



#include <functional>

template <class K>
class CXX0XHash : public std::hash<K> {};

template <>
class CXX0XHash<const char*> {
public:
	size_t operator()(const char* s) const { return strHash(std::string(s)); }
private:
	CXX0XHash<std::string> strHash;
};

template <>
class CXX0XHash<boost::shared_array<const char> > {
public:
	size_t operator()(boost::shared_array<const char> s) const { return strHash(std::string(s.get())); }
private:
	CXX0XHash<std::string> strHash;
};

template <>
class CXX0XHash<CString> {
public:
	size_t operator()(const CString& s) const { return strHash(std::string(s.get())); }
private:
	CXX0XHash<std::string> strHash;
};



#else



template <class K>
class CXX0XHash {};



#endif


#endif /* CXX0XHASH_H_ */
