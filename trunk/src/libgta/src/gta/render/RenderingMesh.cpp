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

#include "RenderingMesh.h"


RenderingMesh::RenderingMesh (
			RenderingPrimitiveFormat primitiveFormat,
			uint32_t flags,
			int vertexCount, uint8_t boneCount,
			Matrix4* boneMatrices,
			int16_t boneIndex,
			GLuint dataBuffer, GLuint indexBuffer,
			int vertexOffset, int vertexStride,
			int submeshIDOffset, int submeshIDStride,
			int normalOffset, int normalStride,
			int texCoordOffset, int texCoordStride,
			int vertexColorOffset, int vertexColorStride,
			int boneIndexOffset, int boneIndexStride,
			int boneWeightOffset, int boneWeightStride
			)
		: primitiveFormat(primitiveFormat), flags(flags), vertexCount(vertexCount),
		  dataBuffer(dataBuffer), indexBuffer(indexBuffer),
		  vertexOffset(vertexOffset), vertexStride(vertexStride),
		  submeshIDOffset(submeshIDOffset), submeshIDStride(submeshIDStride),
		  normalOffset(normalOffset), normalStride(normalStride),
		  texCoordOffset(texCoordOffset), texCoordStride(texCoordStride),
		  vertexColorOffset(vertexColorOffset), vertexColorStride(vertexColorStride),
		  boneIndexOffset(boneIndexOffset), boneIndexStride(boneIndexStride),
		  boneWeightOffset(boneWeightOffset), boneWeightStride(boneWeightStride),
		  modelMatrix(Matrix4::Identity),
		  boneCount(boneCount), boneIndex(boneIndex),
		  boneMatrices(boneMatrices), special(false)
{
}


RenderingMesh::~RenderingMesh()
{
	if (boneMatrices)
		delete[] boneMatrices;

	for (SubmeshIterator it = submeshes.begin() ; it != submeshes.end() ; it++)
		delete *it;
}
