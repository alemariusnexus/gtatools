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

#ifndef RENDERINGMESH_H_
#define RENDERINGMESH_H_

#include <gta/config.h>
#include "RenderingEntity.h"
#include "../resource/texture/TextureSource.h"
#include "ShaderPlugin.h"
#include "RenderingSubmesh.h"
#include <gtaformats/util/math/Matrix4.h>
#include <list>

using std::list;



class RenderingMesh : public RenderingEntity
{
public:
	typedef list<RenderingSubmesh*> SubmeshList;
	typedef SubmeshList::iterator SubmeshIterator;
	typedef SubmeshList::const_iterator ConstSubmeshIterator;

public:
	enum Flags
	{
		HasTransparency = 1 << 0,
		IsAnimated = 1 << 1,
		EnableShaderPluginUniformBuffers = 1 << 2
	};

public:
	RenderingMesh (
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
			);
	virtual ~RenderingMesh();

	void setModelMatrix(const Matrix4& mm) { modelMatrix = mm; }
	void setFlags(uint32_t flags) { this->flags = flags; }

	RenderingPrimitiveFormat getPrimitiveFormat() const { return primitiveFormat; }
	int getVertexCount() const { return vertexCount; }
	GLuint getDataBuffer() const { return dataBuffer; }
	GLuint getIndexBuffer() const { return indexBuffer; }
	int getVertexOffset() const { return vertexOffset; }
	int getVertexStride() const { return vertexStride; }
	int getSubmeshIDOffset() const { return submeshIDOffset; }
	int getSubmeshIDStride() const { return submeshIDStride; }
	int getNormalOffset() const { return normalOffset; }
	int getNormalStride() const { return normalStride; }
	int getTexCoordOffset() const { return texCoordOffset; }
	int getTexCoordStride() const { return texCoordStride; }
	int getVertexColorOffset() const { return vertexColorOffset; }
	int getVertexColorStride() const { return vertexColorStride; }
	int getBoneIndexOffset() const { return boneIndexOffset; }
	int getBoneIndexStride() const { return boneIndexStride; }
	int getBoneWeightOffset() const { return boneWeightOffset; }
	int getBoneWeightStride() const { return boneWeightStride; }
	const Matrix4& getModelMatrix() const { return modelMatrix; }
	uint32_t getFlags() const { return flags; }
	bool hasTransparency() const { return (flags & HasTransparency)  !=  0; }

	uint8_t getBoneCount() const { return boneCount; }
	int16_t getBoneIndex() const { return boneIndex; }
	const Matrix4* getBoneMatrices() const { return boneMatrices; }
	Matrix4* getBoneMatrices() { return boneMatrices; }

	SubmeshIterator getSubmeshBegin() { return submeshes.begin(); }
	ConstSubmeshIterator getSubmeshBegin() const { return submeshes.begin(); }
	SubmeshIterator getSubmeshEnd() { return submeshes.end(); }
	ConstSubmeshIterator getSubmeshEnd() const { return submeshes.end(); }
	size_t getSubmeshCount() const { return submeshes.size(); }

protected:
	RenderingMesh() {}

private:
	void addSubmesh(RenderingSubmesh* sm) { submeshes.push_back(sm); }

protected:
	RenderingPrimitiveFormat primitiveFormat;
	uint32_t flags;

	int vertexCount;

	GLuint dataBuffer;
	GLuint indexBuffer;

	int vertexOffset;
	int vertexStride;
	int submeshIDOffset;
	int submeshIDStride;
	int normalOffset;
	int normalStride;
	int texCoordOffset;
	int texCoordStride;
	int vertexColorOffset;
	int vertexColorStride;
	int boneIndexOffset;
	int boneIndexStride;
	int boneWeightOffset;
	int boneWeightStride;

	Matrix4 modelMatrix;

	uint8_t boneCount;
	int16_t boneIndex;
	Matrix4* boneMatrices;

	SubmeshList submeshes;


	friend class RenderingSubmesh;

public:
	bool special;
};

#endif /* RENDERINGMESH_H_ */
