/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef PVSSECTION_H_
#define PVSSECTION_H_

#include <gtaformats/util/math/intersection.h>
#include <vector>
#include "../StaticSceneObject.h"

using std::vector;


class PVSSection {
public:
	typedef vector<StaticSceneObject*> ObjectList;
	typedef ObjectList::iterator ObjectIterator;

public:
	PVSSection(float x1, float y1, float z1, float x2, float y2, float z2)
			: x1(x1), y1(y1), z1(z1), x2(x2), y2(y2), z2(z2)
			{}
	void getFirstCorner(float& x, float& y, float& z) const { x = x1; y = y1; z = z1; }
	void getSecondCorner(float& x, float& y, float& z) const { x = x2; y = y2; z = z2; }
	bool intersectsSphere(float sx, float sy, float sz, float sr) const
			{ return IntersectAABoxSphere(sx, sy, sz, sr, x1, y1, z1, x2, y2, z2); }
	bool containsPoint(float x, float y, float z) const
			{ return (x >= x1 && x <= x2)  &&  (y >= y1 && y <= y2)  &&  (z >= z1 && z <= z2); }
	void addPotentiallyVisibleObject(StaticSceneObject* object) { pvs.push_back(object); }
	ObjectIterator getPVSObjectBegin() { return pvs.begin(); }
	ObjectIterator getPVSObjectEnd() { return pvs.end(); }
	size_type getPVSObjectCount() { return pvs.size(); }

private:
	PVSSection() {}

private:
	float x1, y1, z1;
	float x2, y2, z2;
	vector<StaticSceneObject*> pvs;

private:
	friend class PVSDatabase;
};

#endif /* PVSSECTION_H_ */
