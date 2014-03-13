/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#ifndef PVSVISIBILITYSET_H_
#define PVSVISIBILITYSET_H_

#include "../parts/PVSSceneObject.h"
#include "PVSDatabase.h"



class PVSVisibilitySet
{
public:
	typedef vector<PVSDatabase::PVSSceneObjectContainer*> ObjectList;
	typedef ObjectList::iterator ObjectIterator;

public:
	PVSVisibilitySet(float dm) : distMultiplier(dm) {}
	void addPotentiallyVisibleObject(PVSDatabase::PVSSceneObjectContainer* object) { pvs.push_back(object); }
	PVSDatabase::PVSSceneObjectIterator getPVSObjectBegin() { return pvs.begin(); }
	PVSDatabase::PVSSceneObjectIterator getPVSObjectEnd() { return pvs.end(); }
	ObjectList::size_type getPVSObjectCount() { return pvs.size(); }
	float getDistanceMultiplier() const { return distMultiplier; }
	void setDistanceMultiplier(float dm) { distMultiplier = dm; }

private:
	float distMultiplier;
	ObjectList pvs;
};

#endif /* PVSVISIBILITYSET_H_ */
