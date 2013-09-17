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

#include "StaticRenderingMesh.h"


StaticRenderingMesh::StaticRenderingMesh (
			RenderingPrimitiveFormat primitiveFormat,
			int vertexCount, int indexCount,
			GLuint dataBuffer, GLuint indexBuffer,
			int normalOffset, int texCoordOffset, int vertexColorOffset,
			GLuint texture
			)
		: primitiveFormat(primitiveFormat),
		  vertexCount(vertexCount), indexCount(indexCount),
		  dataBuffer(dataBuffer), indexBuffer(indexBuffer),
		  normalOffset(normalOffset), texCoordOffset(texCoordOffset), vertexColorOffset(vertexColorOffset),
		  texture(texture),
		  modelMatrix(Matrix4::Identity),
		  transparency(false)
{
	materialColor[0] = 255;
	materialColor[1] = 255;
	materialColor[2] = 255;
	materialColor[3] = 255;
}
