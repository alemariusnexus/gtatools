/*
 * Animator.h
 *
 *  Created on: 01.10.2011
 *      Author: alemariusnexus
 */

#ifndef ANIMATOR_H_
#define ANIMATOR_H_

#include "resource/mesh/MeshClump.h"
#include "Animation.h"


class Animator {
public:
	Animator(MeshClump* clump, Animation* anim) : clump(clump), anim(anim) {}
	void setTime(float time) { this->time = time; updateBoneMatrices(); }
	Matrix4* getBoneMatrices() { return boneMats; }
	Matrix4 getBoneMatrix(size_t idx) { return boneMats[idx]; }

private:
	void updateBoneMatrices();
	void updateBoneMatrix(MeshFrame* frame);

private:
	MeshClump* clump;
	Animation* anim;
	Matrix4* boneMats;
	float time;
};

#endif /* ANIMATOR_H_ */
