/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#include "AnimatedSceneObject.h"
#include <cmath>



AnimatedSceneObject::AnimatedSceneObject(const AnimatedSceneObject& other)
		: def(other.def), time(other.time), modelMatrix(other.modelMatrix), currentAnim(other.currentAnim),
		  currentAnimLength(other.currentAnimLength)
{
}


void AnimatedSceneObject::increaseAnimationTime(float t)
{
	float newTime = time + t;
	time = fmod(newTime, currentAnimLength);
}


void AnimatedSceneObject::setCurrentAnimation(const CString& name)
{
	AnimationPackage* pkg = **def->getAnimationPackagePointer();

	if (pkg) {
		Animation* anim = pkg->find(name);

		if (anim) {
			currentAnimLength = anim->getDuration();
		}
	} else {
		currentAnimLength = 1.0f;
	}

	currentAnim = name;
}