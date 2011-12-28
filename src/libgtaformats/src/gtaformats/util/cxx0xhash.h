/*
 * cxx0xhash.h
 *
 *  Created on: 03.12.2011
 *      Author: alemariusnexus
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
