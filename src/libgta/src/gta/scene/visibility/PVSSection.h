/*
 * PVSBox.h
 *
 *  Created on: 20.03.2011
 *      Author: alemariusnexus
 */

#ifndef PVSSECTION_H_
#define PVSSECTION_H_

#include <gtaformats/util/math/intersection.h>
#include <vector>
#include "../DefaultSceneObject.h"

using std::vector;


class PVSSection {
public:
	typedef vector<DefaultSceneObject*> ObjectList;
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
	void addPotentiallyVisibleObject(DefaultSceneObject* object) { pvs.push_back(object); }
	ObjectIterator getPVSObjectBegin() { return pvs.begin(); }
	ObjectIterator getPVSObjectEnd() { return pvs.end(); }
	size_type getPVSObjectCount() { return pvs.size(); }

private:
	float x1, y1, z1;
	float x2, y2, z2;
	vector<DefaultSceneObject*> pvs;
};

#endif /* PVSSECTION_H_ */
