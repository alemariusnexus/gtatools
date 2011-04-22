/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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
#include "Engine.h"
#include "Submesh.h"
#include <gtaformats/col/COLLoader.h>
#include <cmath>



Mesh::Mesh(int vertexCount, VertexFormat vertexFormat, int flags, const float* vertices, const float* normals,
		const float* texCoords, const uint8_t* vertexColors)
		: flags(flags), vertexFormat(vertexFormat), vertexCount(vertexCount)
{
	init(flags, vertices, normals, texCoords, vertexColors);
}


Mesh::Mesh(const DFFGeometry& geometry, bool autoSubmeshes)
		: vertexCount(geometry.getVertexCount())
{
	flags = 0;
	const float* vertices = geometry.getVertices();
	const float* normals = geometry.getNormals();
	const uint8_t* colors = geometry.getVertexColors();
	const float* texCoords = NULL;

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

	DFFGeometry::ConstMaterialIterator it;
	for (it = geometry.getMaterialBegin() ; it != geometry.getMaterialEnd() ; it++) {
		Material* material = new Material(**it);
		addMaterial(material);
	}

	if (autoSubmeshes) {
		DFFGeometry::ConstPartIterator pit;
		for (pit = geometry.getPartBegin() ; pit != geometry.getPartEnd() ; pit++) {
			Submesh* submesh = new Submesh(this, **pit);
			addSubmesh(submesh);
		}
	}

	const DFFBoundingSphere* b = geometry.getBounds();
	setBounds(b->x, b->y, b->z, b->radius);

	init(flags, vertices, normals, texCoords, colors);
}


Mesh::Mesh(int vertexCount, VertexFormat vertexFormat, int flags, GLuint dataBuffer, int normalOffset,
		int texCoordOffset, int vertexColorOffset)
		: flags(flags), vertexFormat(vertexFormat), vertexCount(vertexCount), dataBuffer(dataBuffer),
		  normalOffs(normalOffset), texCoordOffs(texCoordOffset), vertexColorOffs(vertexColorOffset)
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
}


void Mesh::init(int flags, const float* vertices, const float* normals, const float* texCoords,
		const uint8_t* vertexColors)
{
	int bufferSize = 0;

	if (vertices) {
		bufferSize += vertexCount*3*4;
	}
	if (normals) {
		bufferSize += vertexCount*3*4;
	}
	if (texCoords) {
		bufferSize += vertexCount*2*4;
	}
	if (vertexColors) {
		bufferSize += vertexCount*4;
	}

	glGenBuffers(1, &dataBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, dataBuffer);
	glBufferData(GL_ARRAY_BUFFER, bufferSize, NULL, GL_STATIC_DRAW);

	normalOffs = -1;
	texCoordOffs = -1;
	vertexColorOffs = -1;

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
}


void Mesh::bindDataBuffer()
{
	glBindBuffer(GL_ARRAY_BUFFER, dataBuffer);
}


void Mesh::addSubmesh(Submesh* submesh)
{
	submeshes.push_back(submesh);
}


void Mesh::addMaterial(Material* material)
{
	materials.push_back(material);
}


int Mesh::guessSize() const
{
	int size = sizeof(Mesh) + vertexCount*3*4;

	if ((flags & MeshNormals) != 0) {
		size += vertexCount*3*4;
	}
	if ((flags & MeshTexCoords) != 0) {
		size += vertexCount*2*4;
	}
	if ((flags & MeshVertexColors) != 0) {
		size += vertexCount*4;
	}

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
