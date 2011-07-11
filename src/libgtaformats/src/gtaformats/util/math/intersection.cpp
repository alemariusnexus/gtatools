/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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


#include "intersection.h"


bool IntersectAABoxSphere(float sx, float sy, float sz, float sr,
		float bx1, float by1, float bz1,
		float bx2, float by2, float bz2)
{
	float distSq = 0;

	if (sx < bx1) {
		distSq += (bx1-sx) * (bx1-sx);
	} else if (sx > bx2) {
		distSq += (sx-bx2) * (sx-bx2);
	}

	if (sy < by1) {
		distSq += (by1-sy) * (by1-sy);
	} else if (sy > by2) {
		distSq += (sy-by2) * (sy-by2);
	}

	if (sz < bz1) {
		distSq += (bz1-sz) * (bz1-sz);
	} else if (sz > bz2) {
		distSq += (sz-bz2) * (sz-bz2);
	}

	return distSq <= sr*sr;
}
