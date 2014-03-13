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

#include "MeshAnimator.h"




MeshAnimator::MeshAnimator()
{
}


void MeshAnimator::animateMesh(float* outVertices, float* outNormals, uint32_t vertexCount,
		const float* inVertices, const float* inNormals, const Matrix4& boneMatrix,
		const Matrix4& boneNormalMatrix)
{
	Vector3* voutVertices = (Vector3*) outVertices;

	if (inNormals) {
		Vector3* voutNormals = (Vector3*) outNormals;

		for (uint32_t i = 0 ; i < vertexCount ; i++) {
			Vector3& v = *(((Vector3*) inVertices) + i);
			voutVertices[i] = Vector3(boneMatrix * Vector4(v));

			Vector3& n = *(((Vector3*) inNormals) + i);
			voutNormals[i] = Vector3(boneNormalMatrix * Vector4(n));
		}
	} else {
		for (uint32_t i = 0 ; i < vertexCount ; i++) {
			Vector3& v = *(((Vector3*) inVertices) + i);
			voutVertices[i] = Vector3(boneMatrix * Vector4(v));
		}
	}
}


void MeshAnimator::animateMesh(float* outVertices, float* outNormals, uint32_t vertexCount,
		const float* inVertices, const float* inNormals, const Matrix4& boneMatrix)
{
	if (inNormals) {
		Matrix4 boneNormalMatrix(boneMatrix);
		boneNormalMatrix.invert();
		boneNormalMatrix.transpose();

		animateMesh(outVertices, outNormals, vertexCount, inVertices, inNormals, boneMatrix, boneNormalMatrix);
	} else {
		animateMesh(outVertices, outNormals, vertexCount, inVertices, inNormals, boneMatrix, Matrix4::Identity);
	}
}


void MeshAnimator::animateSkinnedMesh (
		float* outVertices, float* outNormals,
		uint32_t vertexCount,
		float* inVertices, float* inNormals,
		uint8_t* boneIndices, float* boneWeights,
		Matrix4* boneMatrices, Matrix4* boneNormalMatrices,
		uint8_t boneCount
) {
	Vector3* voutVertices = (Vector3*) outVertices;

	if (inNormals) {
		Vector3* voutNormals = (Vector3*) outNormals;

		for (uint32_t i = 0 ; i < vertexCount ; i++) {
			Vector3& v = *(((Vector3*) inVertices) + i);
			voutVertices[i] = Vector3 (
					boneMatrices[boneIndices[i*4]] * boneWeights[i*4] * v
					+ boneMatrices[boneIndices[i*4 + 1]] * boneWeights[i*4 + 1] * v
					+ boneMatrices[boneIndices[i*4 + 2]] * boneWeights[i*4 + 2] * v
					+ boneMatrices[boneIndices[i*4 + 3]] * boneWeights[i*4 + 3] * v
			);

			Vector3& n = *(((Vector3*) inNormals) + i);
			voutNormals[i] = Vector3 (
					boneNormalMatrices[boneIndices[i*4]] * boneWeights[i*4] * n
					+ boneNormalMatrices[boneIndices[i*4 + 1]] * boneWeights[i*4 + 1] * n
					+ boneNormalMatrices[boneIndices[i*4 + 2]] * boneWeights[i*4 + 2] * n
					+ boneNormalMatrices[boneIndices[i*4 + 3]] * boneWeights[i*4 + 3] * n
			);
		}
	} else {
		for (uint32_t i = 0 ; i < vertexCount ; i++) {
			Vector3& v = *(((Vector3*) inVertices) + i);
			voutVertices[i] = Vector3 (
					boneMatrices[boneIndices[i*4]] * boneWeights[i*4] * v
					+ boneMatrices[boneIndices[i*4 + 1]] * boneWeights[i*4 + 1] * v
					+ boneMatrices[boneIndices[i*4 + 2]] * boneWeights[i*4 + 2] * v
					+ boneMatrices[boneIndices[i*4 + 3]] * boneWeights[i*4 + 3] * v
			);
		}
	}
}


void MeshAnimator::animateSkinnedMesh (
		float* outVertices, float* outNormals,
		uint32_t vertexCount,
		float* inVertices, float* inNormals,
		uint8_t* boneIndices, float* boneWeights,
		Matrix4* boneMatrices, uint8_t boneCount
) {
	if (inNormals) {
		Matrix4* boneNormalMatrices = new Matrix4[boneCount];
		memcpy(boneNormalMatrices, boneMatrices, boneCount*sizeof(Matrix4));

		for (uint8_t i = 0 ; i < boneCount ; i++) {
			boneNormalMatrices[i].invert();
			boneNormalMatrices[i].transpose();
		}

		animateSkinnedMesh(outVertices, outNormals, vertexCount, inVertices, inNormals, boneIndices, boneWeights,
				boneMatrices, boneNormalMatrices, boneCount);

		delete[] boneNormalMatrices;
	} else {
		animateSkinnedMesh(outVertices, outNormals, vertexCount, inVertices, inNormals, boneIndices, boneWeights,
				boneMatrices, (Matrix4*) NULL, boneCount);
	}
}
