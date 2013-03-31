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

#ifndef STATICRENDERINGMESH_H_
#define STATICRENDERINGMESH_H_

#include <gta/config.h>
#include "RenderingEntity.h"
#include "../resource/texture/TextureSource.h"
#include <gtaformats/util/math/Matrix4.h>


class StaticRenderingMesh : public RenderingEntity
{

public:
	StaticRenderingMesh (
			RenderingPrimitiveFormat primitiveFormat,
			int vertexCount, int indexCount,
			GLuint dataBuffer, GLuint indexBuffer,
			int normalOffset = -1, int texCoordOffset = -1, int vertexColorOffset = -1,
			GLuint texture = 0
			);
	virtual ~StaticRenderingMesh() {}

	void setModelMatrix(const Matrix4& mm) { modelMatrix = mm; }
	void setMaterialColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
			{ materialColor[0] = r; materialColor[1] = g; materialColor[2] = b; materialColor[3] = a; }
	void setHasTransparency(bool ht) { transparency = ht; }

	RenderingPrimitiveFormat getPrimitiveFormat() const { return primitiveFormat; }
	int getVertexCount() const { return vertexCount; }
	int getIndexCount() const { return indexCount; }
	GLuint getDataBuffer() const { return dataBuffer; }
	GLuint getIndexBuffer() const { return indexBuffer; }
	int getNormalOffset() const { return normalOffset; }
	int getTexCoordOffset() const { return texCoordOffset; }
	int getVertexColorOffset() const { return vertexColorOffset; }
	GLuint getTexture() const { return texture; }
	const Matrix4& getModelMatrix() const { return modelMatrix; }
	void getMaterialColor(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a)
			{ r = materialColor[0]; g = materialColor[1]; b = materialColor[2]; a = materialColor[3]; }
	bool hasTransparency() const { return transparency; }

protected:
	StaticRenderingMesh() {}


protected:
	RenderingPrimitiveFormat primitiveFormat;

	int vertexCount;
	int indexCount;

	GLuint dataBuffer;
	GLuint indexBuffer;

	int normalOffset;
	int texCoordOffset;
	int vertexColorOffset;

	GLuint texture;

	Matrix4 modelMatrix;

	uint8_t materialColor[4];

	bool transparency;
};

#endif /* STATICRENDERINGMESH_H_ */
