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
