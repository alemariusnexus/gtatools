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

#ifndef ANIMATOR_H_
#define ANIMATOR_H_

#include "resource/mesh/MeshClump.h"
#include "Animation.h"
#include <map>

using std::map;


class Animator {
private:
	typedef map<MeshFrame*, Matrix4> FrameMatrixMap;
	typedef map<MeshFrame*, int32_t> PseudoBoneNumMap;

public:
	Animator(MeshClump* clump, Animation* anim);
	virtual ~Animator();
	void setTime(float time) { this->time = fmod(time, anim->getDuration()); updateBoneMatrices(); }
	Matrix4* getBoneMatrices() { return boneMats; }
	Matrix4 getBoneMatrix(size_t idx) { return boneMats[idx]; }
	Matrix4 getFrameMatrix(MeshFrame* frame) { return frameMats[frame]; }
	bool hasPseudoBoneNumbers() const { return !boneNumbersValid; }
	int32_t getPseudoBoneNumber(MeshFrame* frame) const
			{ PseudoBoneNumMap::const_iterator it = pseudoBoneNums.find(frame); return it != pseudoBoneNums.end() ? it->second : -1; }
	int32_t getBoneCount() const { return hasPseudoBoneNumbers() ? pseudoBoneNums.size() : clump->getBoneCount(); }

private:
	void updateBoneMatrices();
	void updateBoneMatrix(MeshFrame* frame);
	bool checkBoneNumberValidity(MeshFrame* frame);
	void generatePseudoBoneNumbers(MeshFrame* frame);

private:
	MeshClump* clump;
	Animation* anim;
	Matrix4* boneMats;
	FrameMatrixMap frameMats;
	float time;
	bool boneNumbersValid;
	PseudoBoneNumMap pseudoBoneNums;
};

#endif /* ANIMATOR_H_ */
