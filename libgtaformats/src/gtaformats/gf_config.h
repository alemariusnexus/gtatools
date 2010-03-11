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

#define GF_USE_SQUISH


#include <istream>


#define SkipBytes(s,c,b,l) \
	for (long long _i = (c) ; _i > 0 ;) {\
		long long skip = _i%(l);\
		skip = (skip == 0) ? (l) : skip;\
		(s)->read((b), (int) skip);\
		_i -= skip;\
	}

/*inline void SkipBytes(istream* s, istream::streamoff c, char* b, int l)
{
	for (std::istream::streamoff i = c ; i > 0 ;) {\
		std::istream::streamoff skip = i % l;\
		skip = (skip == 0) ? l : skip;\
		s->read(b, (int) skip);\
		i -= skip;\
	}
}*/

#endif /* CONFIG_H_ */
