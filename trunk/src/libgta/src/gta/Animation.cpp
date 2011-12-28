/*
 * Animation.cpp
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
 */

#include "Animation.h"
#include <gtaformats/util/strutil.h>
#include <utility>

using std::pair;



Animation::Animation(const Animation& other)
		: size(other.size)
{
	map<const AnimationBone*, CString> nameMap;
	for (BoneMap::const_iterator it = other.boneMap.begin() ; it != other.boneMap.end() ; it++) {
		AnimationBone* bone = new AnimationBone(*it->second);
		boneMap.insert(pair<CString, AnimationBone*>(it->first, bone));
		nameMap.insert(pair<AnimationBone*, CString>(it->second, it->first));
	}

	for (ConstBoneIterator it = other.bones.begin() ; it != other.bones.end() ; it++) {
		const AnimationBone* oldBone = *it;
		CString name = nameMap[oldBone];
		bones.push_back(boneMap[name]);
	}
}


Animation::Animation(const IFPAnimation* anim)
		: size(0)
{
	for (IFPAnimation::ConstObjectIterator it = anim->getObjectBegin() ; it != anim->getObjectEnd() ; it++) {
		const IFPObject* obj = *it;
		AnimationBone* bone = new AnimationBone(obj);
		size += bone->guessSize();
		boneMap.insert(pair<CString, AnimationBone*>(CString(obj->getName()).lower(), bone));
		bones.push_back(bone);
	}
}


Animation::~Animation()
{
	for (BoneMap::iterator it = boneMap.begin() ; it != boneMap.end() ; it++) {
		delete it->second;
	}
}
