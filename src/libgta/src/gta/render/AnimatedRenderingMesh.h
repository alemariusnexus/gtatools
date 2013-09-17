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

#ifndef ANIMATEDRENDERINGMESH_H_
#define ANIMATEDRENDERINGMESH_H_

#include "StaticRenderingMesh.h"


class AnimatedRenderingMesh : public StaticRenderingMesh
{
public:
	AnimatedRenderingMesh (
			RenderingPrimitiveFormat primitiveFormat,
			int vertexCount, int indexCount, int boneCount,
			Matrix4* boneMatrices,
			int16_t boneIdx,
			GLuint dataBuffer, GLuint indexBuffer,
			int normalOffset = -1, int texCoordOffset = -1, int vertexColorOffset = -1,
			int boneIndexOffset = -1, int boneWeightOffset = -1,
			GLuint texture = 0
			);
	virtual ~AnimatedRenderingMesh();

	int getBoneIndexOffset() const { return boneIndexOffset; }
	int getBoneWeightOffset() const { return boneWeightOffset; }
	uint8_t getBoneCount() const { return boneCount; }
	int16_t getBoneIndex() const { return boneIdx; }
	const Matrix4* getBoneMatrices() const { return boneMatrices; }
	Matrix4* getBoneMatrices() { return boneMatrices; }

private:
	int boneIndexOffset;
	int boneWeightOffset;

	uint8_t boneCount;

	int16_t boneIdx;

	Matrix4* boneMatrices;
};

#endif /* ANIMATEDRENDERINGMESH_H_ */
