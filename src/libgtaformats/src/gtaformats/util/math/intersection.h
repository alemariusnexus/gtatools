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

#ifndef INTERSECTION_H_
#define INTERSECTION_H_


bool IntersectAABoxSphere(float sx, float sy, float sz, float sr,
		float bx1, float by1, float bz1,
		float bx2, float by2, float bz2);

inline bool IntersectSphereSphere(float x1, float y1, float z1, float r1, float x2, float y2, float z2, float r2)
{
	float xd = x2-x1;
	float yd = y2-y1;
	float zd = z2-z1;
	float rLen = r1+r2;
	return (xd*xd + yd*yd + zd*zd) <= rLen*rLen;
}


#endif /* INTERSECTION_H_ */
