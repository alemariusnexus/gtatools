/*
	Copyright 2010 David "Alemarius Nexus" Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "DFFGeometry.h"
#include "DFFException.h"
#include "../util/OutOfBoundsException.h"
#include <cstring>



DFFGeometry::DFFGeometry(int32_t numVertices, float* vertices, float* normals, float* uvCoords,
			int8_t uvSetCount, uint8_t* vertexColors)
		: flags(0), uvSetCount(0), vertexCount(numVertices), frameCount(0), ambientLight(0.0f),
		  diffuseLight(0.0f), specularLight(0.0f), associatedFrame(NULL), bounds(NULL)
{
	setVertices(numVertices, vertices, normals, uvCoords, uvSetCount, vertexColors);
}


DFFGeometry::DFFGeometry(const DFFGeometry& other)
		: flags(other.flags), uvSetCount(other.uvSetCount),
		  vertexCount(other.vertexCount), frameCount(other.frameCount),
		  ambientLight(other.ambientLight), diffuseLight(other.diffuseLight),
		  specularLight(other.specularLight),
		  vertexColors(other.vertexColors == NULL ? NULL : new uint8_t[vertexCount*4]),
		  uvCoordSets(other.uvCoordSets == NULL ? NULL : new float[vertexCount*uvSetCount*2]),
		  vertices(new float[vertexCount*3]),
		  normals(other.normals == NULL ? NULL : new float[vertexCount*3]),
		  associatedFrame(other.associatedFrame),
		  bounds(new DFFBoundingSphere)
{
	if (vertexColors) {
		memcpy(vertexColors, other.vertexColors, vertexCount*4);
	}
	if (uvCoordSets) {
		memcpy(uvCoordSets, other.uvCoordSets, vertexCount*uvSetCount*2*4);
	}
	if (normals) {
		memcpy(normals, other.normals, vertexCount*3*4);
	}

	memcpy(vertices, other.vertices, vertexCount*3*4);

	ConstMaterialIterator mit;
	for (mit = other.getMaterialBegin() ; mit != other.getMaterialEnd() ; mit++) {
		materials.push_back(new DFFMaterial(**mit));
	}

	ConstPartIterator pit;
	for (pit = other.getPartBegin() ; pit != other.getPartEnd() ; pit++) {
		parts.push_back(new DFFGeometryPart(**pit));
	}

	memcpy(bounds, other.bounds, sizeof(DFFBoundingSphere));
}


DFFGeometry::~DFFGeometry()
{
	if (vertexColors != NULL) {
		delete[] vertexColors;
	}
	if (uvCoordSets != NULL) {
		/*for (uint8_t i = 0 ; i < uvSetCount ; i++) {
			delete[] uvCoordSets[i];
		}

		delete[] uvCoordSets;*/ // TODO
	}
	if (vertices != NULL) {
		delete[] vertices;
	}
	if (normals != NULL) {
		delete[] normals;
	}

	removeMaterials();
	removeParts();

	/*for (int32_t i = 0 ; i < materialCount ; i++) {
		delete materials[i];
	}

	delete[] materials;


	for (int32_t i = 0 ; i < partCount ; i++) {
		delete parts[i];
	}

	delete[] parts;*/
}


void DFFGeometry::setVertices(int32_t numVertices, float* vertices, float* normals, float* uvCoords,
		int8_t uvSetCount, uint8_t* vertexColors)
{
	if (uvCoords != NULL  &&  uvSetCount == 0) {
		throw DFFException("DFFGeometry::setVertices(): UV coordinates given even though UV set count is 0!",
				__FILE__, __LINE__);
	}

	vertexCount = numVertices;
	this->vertices = vertices;
	this->normals = normals;
	this->uvCoordSets = uvCoords;
	this->uvSetCount = uvSetCount;
	this->vertexColors = vertexColors;
}


void DFFGeometry::mirrorYZ()
{
	if (!vertices && !normals) {
		return;
	}

	for (int32_t i = 0 ; i < vertexCount ; i++) {
		if (vertices != NULL) {
			float y = vertices[(i*3) + 1];
			float z = vertices[(i*3) + 2];
			vertices[(i*3) + 1] = z;
			vertices[(i*3) + 2] = y;
		}

		if (normals != NULL) {
			float ny = normals[(i*3) + 1];
			float nz = normals[(i*3) + 2];
			normals[(i*3) + 1] = nz;
			normals[(i*3) + 2] = ny;
		}
	}
}


void DFFGeometry::scale(float x, float y, float z)
{
	for (int32_t i = 0 ; i < vertexCount ; i++) {
		vertices[(i*3) + 0] *= x;
		vertices[(i*3) + 1] *= y;
		vertices[(i*3) + 2] *= z;
	}
}


int32_t DFFGeometry::indexOf(DFFMaterial* mat) const
{
	ConstMaterialIterator it;
	int i = 0;

	for (it = materials.begin() ; it != materials.end() ; it++, i++) {
		if (*it == mat) {
			return i;
		}
	}

	return -1;
}


void DFFGeometry::removeMaterial(DFFMaterial* material)
{
	MaterialIterator it;

	for (it = materials.begin() ; it != materials.end() ; it++) {
		if (*it == material) {
			materials.erase(it);
			return;
		}
	}
}


void DFFGeometry::removeMaterials()
{
	materials.clear();
}


void DFFGeometry::removePart(DFFGeometryPart* part)
{
	PartIterator it;

	for (it = parts.begin() ; it != parts.end() ; it++) {
		if (*it == part) {
			parts.erase(it);
			part->changeGeometry(NULL);
			return;
		}
	}
}


void DFFGeometry::removeParts()
{
	parts.clear();
}


float* DFFGeometry::getUVCoordSet(uint8_t idx)
{
	if (idx < 0  ||  idx >= uvSetCount) {
		throw OutOfBoundsException(idx, __FILE__, __LINE__);
	}

	return uvCoordSets+idx*vertexCount;
}


const float* DFFGeometry::getUVCoordSet(uint8_t idx) const
{
	if (idx < 0  ||  idx >= uvSetCount) {
		throw OutOfBoundsException(idx, __FILE__, __LINE__);
	}

	return uvCoordSets+idx*vertexCount;
}


DFFMaterial* DFFGeometry::getMaterial(int index)
{
	if (index < 0  ||  index >= materials.size()) {
		throw OutOfBoundsException(index, __FILE__, __LINE__);
	}

	return materials[index];
}


const DFFMaterial* DFFGeometry::getMaterial(int index) const
{
	if (index < 0  ||  index >= materials.size()) {
		throw OutOfBoundsException(index, __FILE__, __LINE__);
	}

	return materials[index];
}


DFFGeometryPart* DFFGeometry::getPart(int index)
{
	if (index < 0  ||  index >= parts.size()) {
		throw OutOfBoundsException(index, __FILE__, __LINE__);
	}

	return parts[index];
}


const DFFGeometryPart* DFFGeometry::getPart(int index) const
{
	if (index < 0  ||  index >= parts.size()) {
		throw OutOfBoundsException(index, __FILE__, __LINE__);
	}

	return parts[index];
}
