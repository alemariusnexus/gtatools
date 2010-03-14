/*
	Copyright 2010 David Lerch

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
