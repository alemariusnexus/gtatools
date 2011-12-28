/*
 * Animator.cpp
 *
 *  Created on: 01.10.2011
 *      Author: alemariusnexus
 */

#include "Animator.h"



void Animator::updateBoneMatrix(MeshFrame* frame)
{
	AnimationBone* bone = anim->getBone(frame->getName());

	if (bone) {
		size_t boneNum = anim->indexOf(bone);

		Matrix4 bm = frame->getModelMatrix() * bone->getInterpolatedFrameMatrix(time);

		if (frame->getParent()) {
			AnimationBone* pbone = anim->getBone(frame->getParent()->getName());

			if (pbone) {
				size_t pboneNum = anim->indexOf(pbone);

				boneMats[boneNum] = boneMats[pboneNum] * bm;
			} else {
				boneMats[boneNum] = bm;
			}
		} else {
			boneMats[boneNum] = bm;
		}
	}

	for (MeshFrame::ChildIterator it = frame->getChildBegin() ; it != frame->getChildEnd() ; it++) {
		MeshFrame* child = *it;
		updateBoneMatrix(child);
	}
}


void Animator::updateBoneMatrices()
{
	boneMats = new Matrix4[anim->getBoneCount()];

	for (size_t i = 0 ; i < anim->getBoneCount() ; i++) {
		boneMats[i] = Matrix4::Identity;
	}

	updateBoneMatrix(clump->getRootFrame());
}
