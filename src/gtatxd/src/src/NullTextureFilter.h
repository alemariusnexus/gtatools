/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#ifndef NULLTEXTUREFILTER_H_
#define NULLTEXTUREFILTER_H_

#include "TextureFilter.h"


class NullTextureFilter : public TextureFilter {
public:
	virtual bool process(const TXDTextureHeader& tex) { return true; }
};

#endif /* NULLTEXTUREFILTER_H_ */
