/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

	This file is part of gtatools-test.

	gtatools-test is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-test is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-test.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef ANIMATEDBONE_H_
#define ANIMATEDBONE_H_

#include <gtaformats/ifp/IFPAnimation.h>
#include <gtaformats/ifp/IFPObject.h>
#include <gtaformats/ifp/IFPFrame.h>
#include <gtaformats/util/math/Matrix4.h>
#include <gta/scene/StaticSceneObject.h>
#include <vector>



class AnimatedBone {
public:
	typedef vector<AnimatedBone*> ChildList;
	typedef ChildList::iterator ChildIterator;
	typedef ChildList::const_iterator ConstChildIterator;

public:
	AnimatedBone(DFFFrame* frame, IFPAnimation* anim);
	void render(float relTime, const Matrix4& parentMat = Matrix4(), const Matrix4& parentFrameMat = Matrix4());

private:
	Matrix4 frameMatrix;
	int32_t frameCount;
	bool rootFrames;
	IFPFrame** frames;
	ChildList children;
	StaticSceneObject* sobj;
	Quaternion lastRotation;
};

#endif /* ANIMATEDBONE_H_ */
