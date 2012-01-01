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

		//Matrix4 bm = bone->getInterpolatedFrameMatrix(time);
		//Matrix4 bm = frame->getModelMatrix();
		Matrix4 modelMat = frame->getModelMatrix();
		Matrix4 ipFrameMatrix = bone->getInterpolatedFrameMatrix(time);
		const float* ipfma = ipFrameMatrix.toArray();

		if (ipfma[12] != 0.0f  ||  ipfma[13] != 0.0f  ||  ipfma[14] != 0.0f) {
			modelMat.setTranslation(0.0f, 0.0f, 0.0f);
		}

		const float* mma = modelMat.toArray();

		modelMat = Matrix4 (
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				mma[12], mma[13], mma[14], 1.0f
		);

		Matrix4 bm = modelMat * bone->getInterpolatedFrameMatrix(time);
		//Matrix4 bm = frame->getAbsoluteModelMatrix() * bone->getInterpolatedFrameMatrix(time);

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
