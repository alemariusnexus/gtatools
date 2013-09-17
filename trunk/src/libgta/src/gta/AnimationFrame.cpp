/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#include "AnimationFrame.h"



AnimationFrame::AnimationFrame(float start, const Quaternion& rot, const Vector3& trans, const Vector3& scale)
		: start(start), rot(rot), trans(trans), scale(scale)
{
}


AnimationFrame::AnimationFrame(const AnimationFrame& other)
		: start(other.start), rot(other.rot), trans(other.trans), scale(other.scale)
{
}


AnimationFrame::AnimationFrame(const IFPRotFrame* frame)
		: start(frame->getTime()), rot(frame->getRotation()), scale(Vector3::One)
{
}


AnimationFrame::AnimationFrame(const IFPRotTransFrame* frame)
		: start(frame->getTime()), rot(frame->getRotation()), trans(frame->getTranslation()),
		  scale(Vector3::One)
{
}


AnimationFrame::AnimationFrame(const IFPRotTransScaleFrame* frame)
		: start(frame->getTime()), rot(frame->getRotation()), trans(frame->getTranslation()),
		  scale(frame->getScale())
{
}
