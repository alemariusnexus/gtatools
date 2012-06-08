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

#ifndef ANIMATIONBONE_H_
#define ANIMATIONBONE_H_

#include <gta/config.h>
#include "AnimationFrame.h"
#include "Engine.h"
#include <vector>
#include <gtaformats/ifp/IFPObject.h>
#include <gtaformats/util/math/Matrix4.h>

using std::vector;



class AnimationBone {
public:
	enum Flags
	{
		FrameHasTranslation = (1 << 0),
		FrameHasScale = (1 << 1)
	};

public:
	typedef vector<AnimationFrame*> FrameList;
	typedef FrameList::iterator FrameIterator;
	typedef FrameList::const_iterator ConstFrameIterator;

public:
	AnimationBone(int flags = 0);
	AnimationBone(const AnimationBone& other);
	AnimationBone(const IFPObject* obj);
	~AnimationBone();
	FrameIterator getFrameBegin() { return frames.begin(); }
	ConstFrameIterator getFrameBegin() const { return frames.begin(); }
	FrameIterator getFrameEnd() { return frames.end(); }
	ConstFrameIterator getFrameEnd() const { return frames.end(); }
	size_t getFrameCount() const { return frames.size(); }
	bool getFrames(float time, AnimationFrame*& f1, AnimationFrame*& f2, float& t) const;
	AnimationFrame* getInterpolatedFrame(float time) const;
	Matrix4 getInterpolatedFrameMatrix(float time) const;
	size_t guessSize() const { return frames.size() * 8 * sizeof(float) + sizeof(int) + sizeof(FrameList); }
	CString getName() const { return name; }
	int32_t getID() const { return id; }
	int getFlags() const { return flags; }

private:
	int flags;
	FrameList frames;
	CString name;
	int32_t id;
};

#endif /* ANIMATIONBONE_H_ */
