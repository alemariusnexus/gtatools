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

#ifndef DEFAULTRENDERINGMESH_H_
#define DEFAULTRENDERINGMESH_H_

#include "RenderingMesh.h"
#include "RenderingEntityGenerator.h"
#include "../resource/mesh/MeshPointer.h"
#include "../resource/texture/TextureSource.h"


class DefaultRenderingMesh : public RenderingMesh
{
public:
	DefaultRenderingMesh (
			RenderingPrimitiveFormat primitiveFormat,
			uint32_t flags,
			int vertexCount, uint8_t boneCount,
			Matrix4* boneMatrices,
			int16_t boneIndex,
			GLuint dataBuffer, GLuint indexBuffer,
			int vertexOffset = 0, int vertexStride = 0,
			int submeshIDOffset = -1, int submeshIDStride = -1,
			int normalOffset = -1, int normalStride = -1,
			int texCoordOffset = -1, int texCoordStride = -1,
			int vertexColorOffset = -1, int vertexColorStride = -1,
			int boneIndexOffset = -1, int boneIndexStride = -1,
			int boneWeightOffset = -1, int boneWeightStride = -1
			)
			: RenderingMesh(primitiveFormat, flags, vertexCount, boneCount, boneMatrices, boneIndex, dataBuffer, indexBuffer,
					vertexOffset, vertexStride, submeshIDOffset, submeshIDStride, normalOffset, normalStride,
					texCoordOffset, texCoordStride, vertexColorOffset, vertexColorStride, boneIndexOffset, boneIndexStride,
					boneWeightOffset, boneWeightStride),
			  meshPtr(NULL)
			{}
	virtual ~DefaultRenderingMesh()
	{
		if (meshPtr) {
			meshPtr->release();
			delete meshPtr;
		}
	}

private:
	DefaultRenderingMesh() {}

private:
	MeshPointer* meshPtr;


	friend class RenderingEntityGenerator;
};

#endif /* DEFAULTRENDERINGMESH_H_ */
