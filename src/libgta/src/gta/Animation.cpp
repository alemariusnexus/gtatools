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
	for (ConstBoneIterator it = other.bones.begin() ; it != other.bones.end() ; it++) {
		AnimationBone* bone = new AnimationBone(**it);
		bones.push_back(bone);
		boneNameMap.insert(pair<CString, AnimationBone*>(bone->getName(), bone));
		boneIDMap.insert(pair<int32_t, AnimationBone*>(bone->getID(), bone));
	}
}


Animation::Animation(const IFPAnimation* anim)
		: size(0)
{
	for (IFPAnimation::ConstObjectIterator it = anim->getObjectBegin() ; it != anim->getObjectEnd() ; it++) {
		const IFPObject* obj = *it;
		AnimationBone* bone = new AnimationBone(obj);
		size += bone->guessSize();
		boneNameMap.insert(pair<CString, AnimationBone*>(obj->getName().lower(), bone));
		boneIDMap.insert(pair<int32_t, AnimationBone*>(obj->getBoneID(), bone));
		bones.push_back(bone);
	}
}


Animation::~Animation()
{
	for (BoneIterator it = bones.begin() ; it != bones.end() ; it++) {
		delete *it;
	}
}


AnimationBone* Animation::getBoneForFrame(MeshFrame* frame)
{
	if (frame->getBoneID() != -1) {
		AnimationBone* bone = getBoneByID(frame->getBoneID());

		if (!bone) {
			return getBoneByName(frame->getName());
		}

		return bone;
	} else {
		return getBoneByName(frame->getName());
	}
}
