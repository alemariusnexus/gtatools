/*
 * AnimatedSceneObject.cpp
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
 */

#include "AnimatedSceneObject.h"
#include <cmath>



void AnimatedSceneObject::increaseAnimationTime(float t)
{
	float newTime = time + t;
	time = fmod(newTime, currentAnimLength);
}


void AnimatedSceneObject::setCurrentAnimation(const CString& name)
{
	AnimationPackage* pkg = **def->getAnimationPackagePointer();
	Animation* anim = pkg->find(name);

	if (anim) {
		currentAnimLength = anim->getDuration();
	}

	currentAnim = name;
}
