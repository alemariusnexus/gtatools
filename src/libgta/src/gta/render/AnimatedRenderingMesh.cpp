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

#include "AnimatedRenderingMesh.h"



AnimatedRenderingMesh::AnimatedRenderingMesh (
			RenderingPrimitiveFormat primitiveFormat,
			int vertexCount, int indexCount, int boneCount,
			Matrix4* boneMatrices,
			int16_t boneIdx,
			GLuint dataBuffer, GLuint indexBuffer,
			int normalOffset, int texCoordOffset, int vertexColorOffset,
			int boneIndexOffset, int boneWeightOffset,
			GLuint texture
			)
		: StaticRenderingMesh(primitiveFormat, vertexCount, indexCount, dataBuffer, indexBuffer,
				normalOffset, texCoordOffset, vertexColorOffset, texture),
		  boneCount(boneCount), boneMatrices(boneMatrices), boneIdx(boneIdx),
		  boneIndexOffset(boneIndexOffset), boneWeightOffset(boneWeightOffset)
{
}


AnimatedRenderingMesh::~AnimatedRenderingMesh()
{
	delete[] boneMatrices;
}
