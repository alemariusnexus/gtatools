/*
 * AnimationPackage.cpp
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
 */

#include "AnimationPackage.h"
#include <utility>

using std::pair;



AnimationPackage::~AnimationPackage()
{
	for (AnimMap::iterator it = anims.begin() ; it != anims.end() ; it++) {
		delete it->second;
	}
}


void AnimationPackage::addAnimation(const CString& name, Animation* anim)
{
	size += anim->guessSize();
	anims.insert(pair<CString, Animation*>(name, anim));
}


Animation* AnimationPackage::find(const CString& name)
{
	AnimMap::iterator it = anims.find(name);

	if (it == anims.end())
		return NULL;

	return it->second;
}
