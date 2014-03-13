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

#ifndef MESHANIMATOR_H_
#define MESHANIMATOR_H_

#include <gta/config.h>
#include <nxcommon/math/Matrix4.h>
#include <nxcommon/file/File.h>


class MeshAnimator
{
public:
	MeshAnimator();
	void animateMesh(float* outVertices, float* outNormals, uint32_t vertexCount, const float* inVertices,
			const float* inNormals, const Matrix4& boneMatrix, const Matrix4& boneNormalMatrix);
	void animateMesh(float* outVertices, float* outNormals, uint32_t vertexCount, const float* inVertices,
			const float* inNormals, const Matrix4& boneMatrix);

	void animateSkinnedMesh(float* outVertices, float* outNormals,
			uint32_t vertexCount,
			float* inVertices, float* inNormals,
			uint8_t* boneIndices, float* boneWeights,
			Matrix4* boneMatrices, Matrix4* boneNormalMatrices,
			uint8_t boneCount);
	void animateSkinnedMesh(float* outVertices, float* outNormals,
			uint32_t vertexCount,
			float* inVertices, float* inNormals,
			uint8_t* boneIndices, float* boneWeights,
			Matrix4* boneMatrices, uint8_t boneCount);
};

#endif /* MESHANIMATOR_H_ */
