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

#include "Mesh.h"
#include "../../Engine.h"
#include "../../render/Renderer.h"
#include "Submesh.h"
#include <gtaformats/col/COLLoader.h>
#include <cmath>



Mesh::Mesh(int vertexCount, VertexFormat vertexFormat, int flags, const float* vertices, const float* normals,
		const float* texCoords, const uint8_t* vertexColors, const uint8_t* boneIndices,
		const float* boneWeights)
		: flags(flags), vertexFormat(vertexFormat), vertexCount(vertexCount), frame(NULL)
{
	init(flags, vertices, normals, texCoords, vertexColors, boneIndices, boneWeights);
}


Mesh::Mesh(const DFFGeometry& geometry, bool autoSubmeshes)
		: vertexCount(geometry.getVertexCount()), frame(NULL)
{
	flags = 0;
	const float* vertices = geometry.getVertices();
	const float* normals = geometry.getNormals();
	const uint8_t* colors = geometry.getVertexColors();
	const float* texCoords = NULL;
	const uint8_t* boneIndices = geometry.getBoneIndices();
	const float* boneWeights = geometry.getBoneWeights();

	if (geometry.getUVSetCount() > 0) {
		texCoords = geometry.getUVCoordSet(0);
	}

	if (geometry.isTriangleStripFormat()) {
		vertexFormat = VertexFormatTriangleStrips;
	} else {
		vertexFormat = VertexFormatTriangles;
	}

	if (normals) {
		flags |= MeshNormals;
	}
	if (texCoords) {
		flags |= MeshTexCoords;
	}
	if (colors) {
		flags |= MeshVertexColors;
	}
	if (boneIndices) {
		flags |= MeshSkinData;
	}

	if (geometry.isDynamicLightingEnabled()) {
		flags |= MeshDynamicLighting;
	}

	DFFGeometry::ConstMaterialIterator it;
	for (it = geometry.getMaterialBegin() ; it != geometry.getMaterialEnd() ; it++) {
		Material* material = new Material(**it);
		addMaterial(material);
	}

	if (autoSubmeshes) {
		DFFGeometry::ConstPartIterator pit;
		for (pit = geometry.getPartBegin() ; pit != geometry.getPartEnd() ; pit++) {
			Submesh* submesh = new Submesh(this, **pit);
		}
	}

	const DFFBoundingSphere* b = geometry.getBounds();
	setBounds(b->x, b->y, b->z, b->radius);

	init(flags, vertices, normals, texCoords, colors, boneIndices, boneWeights);
}


Mesh::Mesh(int vertexCount, VertexFormat vertexFormat, int flags,
		GLuint dataBuffer, GLuint dataBufferSize,
		int vertexOffset, int vertexStride,
		int normalOffset, int normalStride,
		int texCoordOffset, int texCoordStride,
		int vertexColorOffset, int vertexColorStride,
		int boneIndexOffset, int boneIndexStride,
		int boneWeightOffset, int boneWeightStride)
		: flags(flags), vertexFormat(vertexFormat), vertexCount(vertexCount),
		  dataBuffer(dataBuffer), dataBufferSingleSize(dataBufferSize), indexBuffer(0),
		  isLinked(false),
		  vertexOffs(vertexOffset), vertexStride(vertexStride),
		  normalOffs(normalOffset), normalStride(normalStride),
		  texCoordOffs(texCoordOffset), texCoordStride(texCoordStride),
		  vertexColorOffs(vertexColorOffset), vertexColorStride(vertexColorStride),
		  boneIndexOffs(boneIndexOffset), boneIndexStride(boneIndexStride),
		  boneWeightOffs(boneWeightOffset), boneWeightStride(boneWeightStride),
		  frame(NULL)
{
}


Mesh::~Mesh()
{
	vector<Submesh*>::iterator sit;
	for (sit = submeshes.begin() ; sit != submeshes.end() ; sit++) {
		delete *sit;
	}

	vector<Material*>::iterator mit;
	for (mit = materials.begin() ; mit != materials.end() ; mit++) {
		delete *mit;
	}

	glDeleteBuffers(1, &dataBuffer);
	glDeleteBuffers(1, &indexBuffer);
}


int Mesh::getDataBufferSize() const
{
	int bufferSize = 0;

	bufferSize += vertexCount*3*4;

	if (normalOffs != -1) {
		bufferSize += vertexCount*3*4;
	}
	if (texCoordOffs != -1) {
		bufferSize += vertexCount*2*4;
	}
	if (vertexColorOffs != -1) {
		bufferSize += vertexCount*4;
	}
	if (boneIndexOffs != -1) {
		bufferSize += vertexCount*4;
	}
	if (boneWeightOffs != -1) {
		bufferSize += vertexCount*4*4;
	}

	return bufferSize;
}


void Mesh::init(int flags, const float* vertices, const float* normals, const float* texCoords,
		const uint8_t* vertexColors, const uint8_t* boneIndices, const float* boneWeights)
{
	int bufferSize = 0;

	bufferSize += vertexCount*3*4;

	bufferSize += vertexCount;

	if (normals) {
		bufferSize += vertexCount*3*4;
	}
	if (texCoords) {
		bufferSize += vertexCount*2*4;
	}
	if (vertexColors) {
		bufferSize += vertexCount*4;
	}
	if (boneIndices) {
		bufferSize += vertexCount*4;
	}
	if (boneWeights) {
		bufferSize += vertexCount*4*4;
	}

	dataBufferSingleSize = bufferSize;

	char* tmpBuf = new char[bufferSize];

	vertexOffs = -1;
	normalOffs = -1;
	texCoordOffs = -1;
	vertexColorOffs = -1;
	boneIndexOffs = -1;
	boneWeightOffs = -1;

	size_t currentOffs = 0;

	if (vertices) {
		vertexOffs = currentOffs;
		currentOffs += 3*4;
	}

	submeshIDOffs = currentOffs;
	currentOffs += 1;

	if (normals) {
		normalOffs = currentOffs;
		currentOffs += 3*4;
	}
	if (texCoords) {
		texCoordOffs = currentOffs;
		currentOffs += 2*4;
	}
	if (vertexColors) {
		vertexColorOffs = currentOffs;
		currentOffs += 4;
	}
	if (boneIndices) {
		boneIndexOffs = currentOffs;
		currentOffs += 4;
	}
	if (boneWeights) {
		boneWeightOffs += currentOffs;
		currentOffs += 4*4;
	}

	/*vertexStride = currentOffs - 3*4;
	normalStride = currentOffs - 3*4;
	texCoordStride = currentOffs - 2*4;
	vertexColorStride = currentOffs - 4;
	boneIndexStride = currentOffs - 4;
	boneWeightStride = currentOffs - 4*4;*/

	vertexStride = currentOffs;
	submeshIDStride = currentOffs;
	normalStride = currentOffs;
	texCoordStride = currentOffs;
	vertexColorStride = currentOffs;
	boneIndexStride = currentOffs;
	boneWeightStride = currentOffs;

	if (vertices) {
		for (int i = 0 ; i < vertexCount ; i++) {
			memcpy(tmpBuf + vertexOffs + i*currentOffs, vertices+i*3, 3*4);
		}
	}
	for (int i = 0 ; i < vertexCount ; i++) {
		tmpBuf[submeshIDOffs + i*currentOffs] = 0;
	}
	if (normals) {
		for (int i = 0 ; i < vertexCount ; i++) {
			memcpy(tmpBuf + normalOffs + i*currentOffs, normals+i*3, 3*4);
		}
	}
	if (texCoords) {
		for (int i = 0 ; i < vertexCount ; i++) {
			memcpy(tmpBuf + texCoordOffs + i*currentOffs, texCoords+i*2, 2*4);
		}
	}
	if (vertexColors) {
		for (int i = 0 ; i < vertexCount ; i++) {
			memcpy(tmpBuf + vertexColorOffs + i*currentOffs, vertexColors+i*4, 4);
		}
	}
	if (boneIndices) {
		for (int i = 0 ; i < vertexCount ; i++) {
			memcpy(tmpBuf + boneIndexOffs + i*currentOffs, boneIndices+i*4, 4);
		}
	}
	if (boneWeights) {
		for (int i = 0 ; i < vertexCount ; i++) {
			memcpy(tmpBuf + boneWeightOffs + i*currentOffs, boneWeights+i*4, 4*4);
		}
	}

	glGenBuffers(1, &dataBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, dataBuffer);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, tmpBuf, GL_STATIC_DRAW);
	Engine::getInstance()->increaseTestMem(bufferSize, __FILE__, __LINE__);

	delete[] tmpBuf;

	isLinked = false;

	indexBuffer = 0;

	/*int bufferSize = 0;

	bufferSize += vertexCount*3*4;

	if (normals) {
		bufferSize += vertexCount*3*4;
	}
	if (texCoords) {
		bufferSize += vertexCount*2*4;
	}
	if (vertexColors) {
		bufferSize += vertexCount*4;
	}
	if (boneIndices) {
		bufferSize += vertexCount*4;
	}
	if (boneWeights) {
		bufferSize += vertexCount*4*4;
	}

	glGenBuffers(1, &dataBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, dataBuffer);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_STATIC_DRAW);
	Engine::getInstance()->increaseTestMem(bufferSize, __FILE__, __LINE__);

	normalOffs = -1;
	texCoordOffs = -1;
	vertexColorOffs = -1;
	boneIndexOffs = -1;
	boneWeightOffs = -1;

	int offset = 0;

	if (vertices) {
		glBufferSubData(GL_ARRAY_BUFFER, offset, vertexCount*3*4, vertices);
		offset += vertexCount*3*4;
	}
	if (normals) {
		normalOffs = offset;
		glBufferSubData(GL_ARRAY_BUFFER, offset, vertexCount*3*4, normals);
		offset += vertexCount*3*4;
	}
	if (texCoords) {
		texCoordOffs = offset;
		glBufferSubData(GL_ARRAY_BUFFER, offset, vertexCount*2*4, texCoords);
		offset += vertexCount*2*4;
	}
	if (vertexColors) {
		vertexColorOffs = offset;
		glBufferSubData(GL_ARRAY_BUFFER, offset, vertexCount*4, vertexColors);
		offset += vertexCount*4;
	}
	if (boneIndices) {
		boneIndexOffs = offset;
		glBufferSubData(GL_ARRAY_BUFFER, offset, vertexCount*4, boneIndices);
		offset += vertexCount*4;
	}
	if (boneWeights) {
		boneWeightOffs = offset;
		glBufferSubData(GL_ARRAY_BUFFER, offset, vertexCount*4*4, boneWeights);
		offset += vertexCount*4*4;
	}*/
}


void Mesh::link()
{
	if (isLinked)
		return;


	// Build the data buffer

	size_t numSubmeshes = submeshes.size();

	glBindBuffer(GL_ARRAY_BUFFER, dataBuffer);
	char* bufData = (char*) glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);

	char* newData = new char[dataBufferSingleSize * numSubmeshes];

	for (uint8_t i = 0 ; i < numSubmeshes ; i++) {
		memcpy(newData + i*dataBufferSingleSize, bufData, dataBufferSingleSize);

		for (size_t j = 0 ; j < vertexCount ; j++) {
			newData[i*dataBufferSingleSize + submeshIDOffs + j*submeshIDStride] = i;
		}
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBufferData(GL_ARRAY_BUFFER, dataBufferSingleSize * numSubmeshes, newData, GL_STATIC_DRAW);

	delete[] newData;


	// Build the index buffer

	if (indexBuffer == 0)
		glGenBuffers(1, &indexBuffer);

	bool hasCompiledSubmeshes = false;
	//GLuint bufSize = 0;
	size_t numIndices = 0;

	for (SubmeshIterator it = submeshes.begin() ; it != submeshes.end() ; it++) {
		Submesh* submesh = *it;

		if (submesh->isLinked())
			hasCompiledSubmeshes = true;

		numIndices += submesh->getIndexCount();
	}

	uint32_t* newIndices = new uint32_t[numIndices];

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	uint32_t* oldIndices;

	if (hasCompiledSubmeshes)
		oldIndices = (uint32_t*) glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY);

	size_t offset = 0;
	for (SubmeshIterator it = submeshes.begin() ; it != submeshes.end() ; it++) {
		Submesh* submesh = *it;

		int ic = submesh->getIndexCount();

		if (submesh->isLinked()) {
			memcpy(newIndices + offset, oldIndices + submesh->getIndexOffset(), ic*4);
		} else {
			memcpy(newIndices + offset, submesh->indices, ic*4);
		}

		submesh->setLinked(offset);

		offset += ic;
	}

	if (hasCompiledSubmeshes)
		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*4, newIndices, GL_STATIC_DRAW);

	delete[] newIndices;

	isLinked = true;
}


void Mesh::bindDataBuffer()
{
	link();
	glBindBuffer(GL_ARRAY_BUFFER, dataBuffer);
}


void Mesh::addSubmesh(Submesh* submesh)
{
	submeshes.push_back(submesh);

	isLinked = false;
}


void Mesh::addMaterial(Material* material)
{
	materials.push_back(material);
}


int Mesh::guessSize() const
{
	int size = vertexCount*3*4 + vertexCount;

	if ((flags & MeshNormals) != 0) {
		size += vertexCount*3*4;
	}
	if ((flags & MeshTexCoords) != 0) {
		size += vertexCount*2*4;
	}
	if ((flags & MeshVertexColors) != 0) {
		size += vertexCount*4;
	}
	if ((flags & MeshSkinData) != 0) {
		size += vertexCount*4 + vertexCount*4*4;
	}

	size *= submeshes.size();

	size += sizeof(Mesh);

	vector<Material*>::const_iterator mit;
	for (mit = materials.begin() ; mit != materials.end() ; mit++) {
		size += (*mit)->guessSize();
	}

	vector<Submesh*>::const_iterator sit;
	for (sit = submeshes.begin() ; sit != submeshes.end() ; sit++) {
		size += (*sit)->guessSize();
	}

	return size;
}
