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

#ifndef ANIMATOR_H_
#define ANIMATOR_H_

#include "resource/mesh/MeshClump.h"
#include "Animation.h"
#include <map>

using std::map;


class Animator {
private:
	typedef map<MeshFrame*, Matrix4> FrameMatrixMap;

public:
	Animator(MeshClump* clump, Animation* anim) : clump(clump), anim(anim), boneMats(NULL) {}
	void setTime(float time) { this->time = time; updateBoneMatrices(); }
	Matrix4* getBoneMatrices() { return boneMats; }
	Matrix4 getBoneMatrix(size_t idx) { return boneMats[idx]; }
	Matrix4 getFrameMatrix(MeshFrame* frame) { return frameMats[frame]; }

private:
	void updateBoneMatrices();
	void updateBoneMatrix(MeshFrame* frame);

private:
	MeshClump* clump;
	Animation* anim;
	Matrix4* boneMats;
	FrameMatrixMap frameMats;
	float time;
};

#endif /* ANIMATOR_H_ */
