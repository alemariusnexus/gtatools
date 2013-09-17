/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#include "PVSSection.h"
#include "PVSVisibilitySet.h"



PVSVisibilitySet* PVSSection::getVisibilitySet(float multiplier)
{
	PVSVisibilitySet* nearestSet = NULL;

	for (SetIterator it = sets.begin() ; it != sets.end() ; it++) {
		PVSVisibilitySet* set = *it;
		float mul = set->getDistanceMultiplier();

		if (mul >= multiplier) {
			nearestSet = set;
		} else {
			break;
		}
	}

	return nearestSet;
}


bool PVSSection::SetComparator::operator()(const PVSVisibilitySet* s1, const PVSVisibilitySet* s2) const
{
	return s1->getDistanceMultiplier() > s2->getDistanceMultiplier();
}
