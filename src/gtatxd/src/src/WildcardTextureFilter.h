/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

	This file is part of gtatxd.

	gtatxd is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatxd is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatxd.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef WILDCARDTEXTUREFILTER_H_
#define WILDCARDTEXTUREFILTER_H_

#include "TextureFilter.h"


class WildcardTextureFilter : public TextureFilter {
public:
	WildcardTextureFilter(const char* pattern);
	virtual bool process(const TXDTextureHeader& tex);

private:
	char* pattern;
};

#endif /* WILDCARDTEXTUREFILTER_H_ */
