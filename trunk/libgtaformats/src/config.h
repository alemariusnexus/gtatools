/*
 * config.h
 *
 *  Created on: 05.01.2010
 *      Author: alemariusnexus
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#ifndef _MSC_VER
#	include <stdint.h>
#else
#	include "msvcstdint.h"
#endif


#define SkipBytes(s,c,b,l) \
	for (int _i = (c) ; _i > 0 ;) {\
		int skip = _i%(l);\
		skip = (skip == 0) ? (l) : skip;\
		(s)->read((b), skip);\
		_i -= skip;\
	}

#endif /* CONFIG_H_ */
