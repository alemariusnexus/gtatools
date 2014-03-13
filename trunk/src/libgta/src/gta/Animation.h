/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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
#include <nxcommon/strutil.h>
#include <nxcommon/CString.h>
#include <map>
#include <vector>
#include <algorithm>
#include "resource/mesh/MeshFrame.h"

using std::map;
using std::find;


/**	\brief Represents a skeletal animation.
 *
 * 	This class represents a skeletal animation. This is an animation consisting of a number of AnimationBones
 * 	and their animation keyframes.
 *
 * 	This class only represents the bone animation and does not contain any link to a mesh structure. To
 * 	associate bones with MeshFrames and to actually run the animation, the Animator class is used.
 */
class Animation {
public:
	typedef map<CString, AnimationBone*> BoneNameMap;
	typedef map<int32_t, AnimationBone*> BoneIDMap;
	typedef vector<AnimationBone*> BoneList;
	typedef BoneList::iterator BoneIterator;
	typedef BoneList::const_iterator ConstBoneIterator;

public:
	/**	\brief Creates an empty animation without any bones.
	 */
	Animation() : size(0) {}

	/**	\brief Copy constructor.
	 *
	 * 	\param other The animation to copy.
	 */
	Animation(const Animation& other);

	/**	\brief Create an Animation object from an IFPAnimation.
	 *
	 *	\param anim The IFPAnimation.
	 */
	Animation(const IFPAnimation* anim);

	/**	\brief Destructor
	 */
	~Animation();

	/**	\brief Returns the AnimationBone with the given name.
	 *
	 * 	\brief name The name of the bone. It is assumed to be lower-case, because all bone names are stored
	 * 		lower-case and searching is done case-sensitive.
	 * 	\return The requested bone or NULL if none was found.
	 */
	AnimationBone* getBoneByName(const CString& name);

	/**	\brief Returns the AnimationBone with the given bone ID.
	 *
	 * 	\param id The bone ID.
	 * 	\return The requested bone or NULL if none was found.
	 */
	AnimationBone* getBoneByID(int32_t id) { return boneIDMap[id]; }

	/**	\brief Returns the bone associated to the given MeshFrame.
	 *
	 * 	This method first tries getBoneByID(int32_t), and if this fails, it tries
	 * 	getBoneByName(const CString&). If either of these calls succeeds, the bone is returned.
	 *
	 * 	\param frame The frame to search a bone for.
	 * 	\return The requested bone or NULL if none was found.
	 */
	AnimationBone* getBoneForFrame(MeshFrame* frame);

	/**	\brief Return the bone begin iterator.
	 *
	 * 	\return The bone begin iterator.
	 */
	BoneIterator getBoneBegin() { return bones.begin(); }

	/**	\brief Return the bone end iterator.
	 *
	 * 	\return The bone end iterator.
	 */
	BoneIterator getBoneEnd() { return bones.end(); }

	/**	\brief Return the bone begin iterator.
	 *
	 * 	\return The bone begin iterator.
	 */
	ConstBoneIterator getBoneBegin() const { return bones.begin(); }

	/**	\brief Return the bone end iterator.
	 *
	 * 	\return The bone end iterator.
	 */
	ConstBoneIterator getBoneEnd() const { return bones.end(); }

	/**	\brief Return the number of bones.
	 *
	 * 	\return The number of bones.
	 */
	size_t getBoneCount() const { return bones.size(); }

	size_t guessSize() const { return size; }

	/**	\brief Return the duration of this animation.
	 *
	 * 	This is the time needed to fully execute the animation once.
	 *
	 * 	\return The animation duration.
	 */
	float getDuration() const { return duration; }

private:
	BoneNameMap boneNameMap;
	BoneIDMap boneIDMap;
	BoneList bones;
	size_t size;
	float duration;
};

#endif /* ANIMATION_H_ */
