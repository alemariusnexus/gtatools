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

#include "AnimatedMapSceneObject.h"



AnimatedMapSceneObject::AnimatedMapSceneObject()
		: MapSceneObject(), time(0.0f), autoAnim(true),
		  autoPickDefaultAnim(false)
{
}


AnimatedMapSceneObject::AnimatedMapSceneObject(const AnimatedMapSceneObject& other)
		: MapSceneObject(other), curAnim(other.curAnim), time(other.time), autoAnim(true),
		  autoPickDefaultAnim(other.autoPickDefaultAnim)
{
}


CString AnimatedMapSceneObject::getCurrentAnimation() const
{
	if (curAnim.get()) {
		return curAnim;
	} else if (autoPickDefaultAnim) {
		ConstLODInstanceMapIterator beg, end;
		getRenderingLODInstances(beg, end);

		if (beg == end) {
			// No rendering LOD instance
			return CString();
		} else {
			for (ConstLODInstanceMapIterator it = beg ; it != end ; it++) {
				const MapSceneObjectLODInstance* lodInst = *it;
				const MapItemDefinition* def = lodInst->getDefinition();

				if (def->getType() == ItemTypeAnimatedMapItem) {
					const AnimatedMapItemDefinition* adef = (const AnimatedMapItemDefinition*) def;
					return adef->getDefaultAnimation();
				}
			}

			return CString();
		}
	} else {
		return CString();
	}
}
