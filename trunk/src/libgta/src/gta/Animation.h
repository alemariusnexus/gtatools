/*
 * Animation.h
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
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
	AnimationBone* getBone(const CString& id) { return boneMap[id]; }
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
