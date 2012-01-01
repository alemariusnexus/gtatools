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

#ifndef ANIMATION_H_
#define ANIMATION_H_

#include <gta/config.h>
#include "AnimationBone.h"
#include <gtaformats/ifp/IFPAnimation.h>
#include <gtaformats/util/strutil.h>
#include <gtaformats/util/CString.h>
#include <map>
#include <vector>
#include <algorithm>

using std::map;
using std::find;


class Animation {
public:
	typedef map<CString, AnimationBone*> BoneMap;
	typedef vector<AnimationBone*> BoneList;
	typedef BoneList::iterator BoneIterator;
	typedef BoneList::const_iterator ConstBoneIterator;

public:
	Animation() : size(0) {}
	Animation(const Animation& other);
	Animation(const IFPAnimation* anim);
	~Animation();
	AnimationBone* getBone(const CString& id) { if (id.get() == NULL) return NULL; return boneMap[id]; }
	BoneIterator getBoneBegin() { return bones.begin(); }
	BoneIterator getBoneEnd() { return bones.end(); }
	ConstBoneIterator getBoneBegin() const { return bones.begin(); }
	ConstBoneIterator getBoneEnd() const { return bones.end(); }
	size_t indexOf(AnimationBone* bone) const { return find(bones.begin(), bones.end(), bone) - bones.begin(); }
	size_t getBoneCount() const { return bones.size(); }
	size_t guessSize() const { return size; }
	float getDuration() const { return bones.empty() ? 0.0f : (*((*bones.begin())->getFrameEnd()-1))->getStart(); }

private:
	BoneMap boneMap;
	BoneList bones;
	size_t size;
};

#endif /* ANIMATION_H_ */
