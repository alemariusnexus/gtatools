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

#ifndef MESH_H_
#define MESH_H_

#include <gta/config.h>
#include "../../gl.h"
#include <vector>
#include <gtaformats/dff/DFFGeometry.h>
#include <gtaformats/col/COLModel.h>
#include "Material.h"
#include "MeshFrame.h"

using std::vector;



class Submesh;


enum MeshFlags
{
	MeshNormals = 1<<0,
	MeshTexCoords = 1<<1,
	MeshVertexColors = 1<<2,
	MeshTriangleStrips = 1<<3,
	MeshSkinData = 1<<4,
	MeshDynamicLighting = 1<<5
};


enum VertexFormat
{
	VertexFormatPoints,
	VertexFormatLines,
	VertexFormatTriangles,
	VertexFormatTriangleStrips
};


class Mesh {
public:
	typedef vector<Submesh*>::iterator SubmeshIterator;
	typedef vector<Material*>::iterator MaterialIterator;

public:
	Mesh(int vertexCount, VertexFormat vertexFormat, int flags, const float* vertices,
			const float* normals = NULL, const float* texCoords = NULL, const uint8_t* vertexColors = NULL,
			const uint8_t* boneIndices = NULL, const float* boneWeights = NULL);
	Mesh(const DFFGeometry& geometry, bool autoSubmeshes = true);
	Mesh(int vertexCount, VertexFormat vertexFormat, int flags,
			GLuint dataBuffer, GLuint dataBufferSize,
			int vertexOffset = 0, int vertexStride = 0,
			int normalOffset = -1, int normalStride = -1,
			int texCoordOffset = -1, int texCoordStride = -1,
			int vertexColorOffset = -1, int vertexColorStride = -1,
			int boneIndexOffset = -1, int boneIndexStride = -1,
			int boneWeightOffset = 0, int boneWeightStride = 0);
	~Mesh();

	int getFlags() const { return flags; }

	void setDynamicLightingEnabled(bool e)
			{ if (e) flags |= MeshDynamicLighting; else flags &= ~MeshDynamicLighting; }

	VertexFormat getVertexFormat() const { return vertexFormat; }

	int getVertexCount() const { return vertexCount; }

	int getVertexOffset() const { return vertexOffs; }

	int getVertexStride() const { return vertexStride; }

	int getSubmeshIDOffset() const { return submeshIDOffs; }

	int getSubmeshIDStride() const { return submeshIDStride; }

	int getNormalOffset() const { return normalOffs; }

	int getNormalStride() const { return normalStride; }

	int getTexCoordOffset() const { return texCoordOffs; }

	int getTexCoordStride() const { return texCoordStride; }

	int getVertexColorOffset() const { return vertexColorOffs; }

	int getVertexColorStride() const { return vertexColorStride; }

	int getBoneIndexOffset() const { return boneIndexOffs; }

	int getBoneIndexStride() const { return boneIndexStride; }

	int getBoneWeightOffset() const { return boneWeightOffs; }

	int getBoneWeightStride() const { return boneWeightStride; }

	GLuint getDataBuffer() const { const_cast<Mesh*>(this)->link(); return dataBuffer; }

	void bindDataBuffer();

	GLuint getIndexBuffer() const { const_cast<Mesh*>(this)->link(); return indexBuffer; }

	int getDataBufferSize() const;

	SubmeshIterator getSubmeshBegin() { return submeshes.begin(); }

	SubmeshIterator getSubmeshEnd() { return submeshes.end(); }

	size_t getSubmeshCount() const { return submeshes.size(); }

	void addMaterial(Material* material);

	MaterialIterator getMaterialBegin() { return materials.begin(); }

	MaterialIterator getMaterialEnd() { return materials.end(); }

	Material* getMaterial(int index) { return materials[index]; }

	float* getBounds() { return bounds; }

	void setBounds(float x, float y, float z, float r) { bounds[0]=x; bounds[1]=y; bounds[2]=z; bounds[3]=r; }

	int guessSize() const;

	MeshFrame* getFrame() { return frame; }

	const MeshFrame* getFrame() const { return frame; }

	void setFrame(MeshFrame* f) { frame = f; }

	void link();

private:
	void init(int flags, const float* vertices, const float* normals, const float* texCoords,
			const uint8_t* vertexColors, const uint8_t* boneIndices, const float* boneWeights);

	void addSubmesh(Submesh* submesh);

private:
	int flags;
	VertexFormat vertexFormat;
	int vertexCount;
	GLuint dataBuffer;
	GLuint dataBufferSingleSize;
	GLuint indexBuffer;
	bool isLinked;
	vector<Submesh*> submeshes;
	vector<Material*> materials;
	float bounds[4];
	int vertexOffs;
	int vertexStride;
	int submeshIDOffs;
	int submeshIDStride;
	int normalOffs;
	int normalStride;
	int texCoordOffs;
	int texCoordStride;
	int vertexColorOffs;
	int vertexColorStride;
	int boneIndexOffs;
	int boneIndexStride;
	int boneWeightOffs;
	int boneWeightStride;
	MeshFrame* frame;

	friend class Submesh;
};

#endif /* MESH_H_ */
