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

#ifndef DEFAULTSTATICRENDERINGMESH_H_
#define DEFAULTSTATICRENDERINGMESH_H_

#include "StaticRenderingMesh.h"
#include "RenderingEntityGenerator.h"
#include "../resource/mesh/MeshPointer.h"
#include "../resource/texture/TextureSource.h"


class DefaultStaticRenderingMesh : public StaticRenderingMesh
{
public:
	DefaultStaticRenderingMesh (
			RenderingPrimitiveFormat primitiveFormat,
			int vertexCount, int indexCount,
			GLuint dataBuffer, GLuint indexBuffer,
			int normalOffset = -1, int texCoordOffset = -1, int vertexColorOffset = -1,
			GLuint texture = 0
			)
			: StaticRenderingMesh(primitiveFormat, vertexCount, indexCount, dataBuffer, indexBuffer,
					normalOffset, texCoordOffset, vertexColorOffset, texture),
			  meshPtr(NULL), texSrc(NULL)
			{}
	virtual ~DefaultStaticRenderingMesh()
	{
		if (meshPtr) {
			meshPtr->release();
			delete meshPtr;
		}

		if (texSrc) {
			texSrc->release();
			delete texSrc;
		}
	}

private:
	DefaultStaticRenderingMesh() {}

private:
	MeshPointer* meshPtr;
	TextureSource* texSrc;


	friend class RenderingEntityGenerator;
};

#endif /* DEFAULTSTATICRENDERINGMESH_H_ */
