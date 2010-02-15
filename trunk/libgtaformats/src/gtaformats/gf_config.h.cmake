/*
 * config.h
 *
 *  Created on: 05.01.2010
 *      Author: alemariusnexus
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#ifdef _MSC_VER
#	pragma warning(disable : 4996)
#endif

#ifndef _MSC_VER
#	include <stdint.h>
#else
#	include "msvcstdint.h"
#endif

#cmakedefine GF_USE_SQUISH


#include <istream>


#define SkipBytes(s,c,b,l) \
	for (std::istream::streamoff _i = (c) ; _i > 0 ;) {\
		std::istream::streamoff skip = _i%(l);\
		skip = (skip == 0) ? (l) : skip;\
		(s)->read((b), (int) skip);\
		_i -= skip;\
	}

#endif /* CONFIG_H_ */
