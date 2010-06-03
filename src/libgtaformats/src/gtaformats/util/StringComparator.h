/*
 * StringComparator.h
 *
 *  Created on: 14.03.2010
 *      Author: alemariusnexus
 */

#ifndef STRINGCOMPARATOR_H_
#define STRINGCOMPARATOR_H_

#include <cstring>


class StringComparator
{
public:
	bool operator()(const char* s1, const char* s2) const
	{
		return strcmp(s1, s2) < 0;
	}
};

#endif /* STRINGCOMPARATOR_H_ */
