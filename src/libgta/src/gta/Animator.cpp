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

#include "Animator.h"




Animator::Animator(MeshClump* clump, Animation* anim)
		: clump(clump), anim(anim), boneMats(NULL)
{
	boneNumbersValid = checkBoneNumberValidity(clump->getRootFrame());

	if (!boneNumbersValid) {
		generatePseudoBoneNumbers(clump->getRootFrame());
	}
}


bool Animator::checkBoneNumberValidity(MeshFrame* frame)
{
	if (frame->getBoneNumber() != -1) {
		return true;
	}

	for (MeshFrame::ChildIterator it = frame->getChildBegin() ; it != frame->getChildEnd() ; it++) {
		MeshFrame* child = *it;

		if (checkBoneNumberValidity(child))
			return true;
	}
}


void Animator::generatePseudoBoneNumbers(MeshFrame* frame)
{
	pseudoBoneNums.insert(pair<MeshFrame*, int32_t>(frame, pseudoBoneNums.size()));

	for (MeshFrame::ChildIterator it = frame->getChildBegin() ; it != frame->getChildEnd() ; it++) {
		MeshFrame* child = *it;
		generatePseudoBoneNumbers(child);
	}
}


void Animator::updateBoneMatrix(MeshFrame* frame)
{
	AnimationBone* bone = NULL;

	if (frame->getName() != CString("root")  ||  frame->getChildByName(CString("root")) == NULL) {
		bone = anim->getBoneForFrame(frame);
	}

	if (bone) {
		Matrix4 modelMat = frame->getModelMatrix();
		Matrix4 ipFrameMatrix = bone->getInterpolatedFrameMatrix(time);

		// IMPORTANT: IFP frames seem to OVERWRITE the local transformation of the DFF frames. Because IFP
		// frames always have a rotation matrix, the DFF frames' rotation is basically ignored. However, only
		// some IFP frames have translation values, and for those without, the translation of the DFF frame is
		// used, otherwise it is ignored just as with the rotation.

		// Ignore rotation, but keep translation
		const float* mma = modelMat.toArray();
		modelMat = Matrix4 (
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				mma[12], mma[13], mma[14], 1.0f
		);

		// If the IFP frame has translation info, the DFF frame's translation is ignored
		if ((bone->getFlags() & AnimationBone::FrameHasTranslation)  !=  0) {
			modelMat.setTranslation(0.0f, 0.0f, 0.0f);
		}

		Matrix4 bm = modelMat * ipFrameMatrix;

		MeshFrame* parent = frame->getParent();

		if (parent) {
			Matrix4 pMat = frameMats[parent];
			frameMats.insert(pair<MeshFrame*, Matrix4>(frame, pMat * bm));
		} else {
			frameMats.insert(pair<MeshFrame*, Matrix4>(frame, bm));
		}
	} else {
		MeshFrame* parent = frame->getParent();

		if (parent) {
			Matrix4 pMat = frameMats[parent];
			frameMats.insert(pair<MeshFrame*, Matrix4>(frame, pMat * frame->getModelMatrix()));
		} else {
			frameMats.insert(pair<MeshFrame*, Matrix4>(frame, frame->getModelMatrix()));
		}
	}

	for (MeshFrame::ChildIterator it = frame->getChildBegin() ; it != frame->getChildEnd() ; it++) {
		MeshFrame* child = *it;
		updateBoneMatrix(child);
	}
}


void Animator::updateBoneMatrices()
{
	frameMats.clear();

	if (boneMats)
		delete[] boneMats;

	int32_t numBones = getBoneCount();

	updateBoneMatrix(clump->getRootFrame());

	boneMats = new Matrix4[numBones];

	for (FrameMatrixMap::iterator it = frameMats.begin() ; it != frameMats.end() ; it++) {
		MeshFrame* frame = it->first;

		it->second = it->second * frame->getInverseBoneMatrix();

		if (frame->getBoneID() != -1) {
			boneMats[frame->getBoneNumber()] = it->second;
		} else if (!boneNumbersValid) {
			PseudoBoneNumMap::iterator pit = pseudoBoneNums.find(frame);

			if (pit != pseudoBoneNums.end()) {
				int num = pit->second;
				boneMats[num] = it->second;
			}
		}
	}
}
