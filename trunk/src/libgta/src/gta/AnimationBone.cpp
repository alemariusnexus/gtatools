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
		: flags(0), name(obj->getName().lower()), id(obj->getBoneID())
{
	if (obj->getFrameType() == IFPObject::RotTransFrame) {
		flags |= FrameHasTranslation;
	} else if (obj->getFrameType() == IFPObject::RotTransScaleFrame) {
		flags |= FrameHasTranslation | FrameHasScale;
	}

	for (IFPObject::ConstFrameIterator it = obj->getFrameBegin() ; it != obj->getFrameEnd() ; it++) {
		const IFPRotFrame* iframe = *it;

		if (obj->getFrameType() == IFPObject::RotTransFrame) {
			frames.push_back(new AnimationFrame((IFPRotTransFrame*) iframe));
		} else if (obj->getFrameType() == IFPObject::RotTransScaleFrame) {
			frames.push_back(new AnimationFrame((IFPRotTransScaleFrame*) iframe));
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
	if (frames.empty()) {
		return false;
	}

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

			float tdiff = f2->getStart() - f1->getStart();

			if (tdiff == 0.0f)
				t = 1.0f;
			else
				t = (time - f1->getStart()) / tdiff;

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

	bool found = getFrames(time, f1, f2, t);

	if (found) {
		Vector3 t1, t2;
		Vector3 s1 = Vector3::One,
				s2 = Vector3::One;
		Quaternion r1 = f1->getRotation();
		Quaternion r2 = f2->getRotation();

		if ((flags & FrameHasTranslation)  !=  0) {
			t1 = f1->getTranslation();
			t2 = f2->getTranslation();
		}
		if ((flags & FrameHasScale)  !=  0) {
			s1 = f1->getScale();
			s2 = f2->getScale();
		}

		Quaternion ir = r1.slerp(r2, t);
		Vector3 it = t1 + (t2-t1) * t;
		Vector3 is = s1 + (s2-s1) * t;

		AnimationFrame* frame = new AnimationFrame(time, ir, it, is);

		return frame;
	} else {
		return NULL;
	}
}


Matrix4 AnimationBone::getInterpolatedFrameMatrix(float time) const
{
	AnimationFrame* frame = getInterpolatedFrame(time);

	if (frame) {
		Matrix4 mat = Matrix4::scale(frame->getScale())
				* Matrix4::translation(frame->getTranslation())
				* Matrix4(frame->getRotation().toMatrix());

		delete frame;

		return mat;
	} else {
		return Matrix4::Identity;
	}
}
