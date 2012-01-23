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

#include "AnimationBone.h"
#include "Engine.h"



AnimationBone::AnimationBone(int flags)
		: flags(flags)
{
}


AnimationBone::AnimationBone(const AnimationBone& other)
		: flags(other.flags), name(other.name), id(other.id)
{
	for (ConstFrameIterator it = other.frames.begin() ; it != other.frames.end() ; it++) {
		frames.push_back(new AnimationFrame(**it));
	}
}


AnimationBone::AnimationBone(const IFPObject* obj)
		: flags(obj->getFrameType() == IFPObject::RootFrame ? FrameHasTranslation : 0),
		  name(obj->getName().lower()), id(obj->getBoneID())
{
	for (IFPObject::ConstFrameIterator it = obj->getFrameBegin() ; it != obj->getFrameEnd() ; it++) {
		const IFPFrame* iframe = *it;

		if ((flags & FrameHasTranslation) != 0) {
			frames.push_back(new AnimationFrame((IFPRootFrame*) iframe));
		} else {
			frames.push_back(new AnimationFrame(iframe));
		}
	}
}


AnimationBone::~AnimationBone()
{
	for (FrameIterator it = frames.begin() ; it != frames.end() ; it++) {
		delete *it;
	}
}


bool AnimationBone::getFrames(float time, AnimationFrame*& f1, AnimationFrame*& f2, float& t) const
{
	for (ConstFrameIterator it = frames.begin() ; it != frames.end() ; it++) {
		AnimationFrame* frame = *it;

		if (time <= frame->getStart()) {
			f2 = frame;

			if (it == frames.begin()) {
				if (frames.size() != 1) {
					f1 = *(frames.end()-1);
				} else {
					f1 = f2;
				}
			} else {
				f1 = *(it-1);
			}

			t = (time - f1->getStart()) / (f2->getStart() - f1->getStart());

			return true;
		}
	}

	// Time is past the start time of the highest frame. We assume that this means fully using the last
	// frame.

	f1 = *(frames.end()-1);
	f2 = f1;
	t = 1.0f;

	return true;
}


AnimationFrame* AnimationBone::getInterpolatedFrame(float time) const
{
	AnimationFrame* f1 = NULL;
	AnimationFrame* f2 = NULL;
	float t;

	getFrames(time, f1, f2, t);

	Vector3 t1, t2;
	Quaternion r1 = f1->getRotation();
	Quaternion r2 = f2->getRotation();

	if ((flags & FrameHasTranslation)  !=  0) {
		t1 = f1->getTranslation();
		t2 = f2->getTranslation();
	}

	Quaternion ir = r1.slerp(r2, t);

	Vector3 it = t1 + (t2-t1) * t;

	AnimationFrame* frame = new AnimationFrame(time, ir, it);

	return frame;
}


Matrix4 AnimationBone::getInterpolatedFrameMatrix(float time) const
{
	AnimationFrame* frame = getInterpolatedFrame(time);
	Matrix4 mat = Matrix4::translation(frame->getTranslation()) * Matrix4(frame->getRotation().toMatrix());
	delete frame;
	return mat;
}
