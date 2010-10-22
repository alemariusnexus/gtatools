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
#include "DFFMesh.h"
#include "../util/OutOfBoundsException.h"
#include <cstring>



DFFGeometry::DFFGeometry(int32_t numVertices, float* vertices, float* normals, float* uvCoords,
			int8_t uvSetCount, uint8_t* vertexColors)
		: flags(0), uvSetCount(0), vertexCount(numVertices), frameCount(0), ambientLight(0.0f),
		  diffuseLight(0.0f), specularLight(0.0f), associatedFrame(NULL), bounds(NULL), mesh(NULL)
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
		  bounds(new DFFBoundingSphere), mesh(NULL)
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
		delete[] uvCoordSets;
	}
	if (vertices != NULL) {
		delete[] vertices;
	}
	if (normals != NULL) {
		delete[] normals;
	}

	MaterialIterator it;
	for (it = materials.begin() ; it != materials.end() ; it++) {
		(*it)->reparent(NULL);
		delete *it;
	}

	PartIterator pit;
	for (pit = parts.begin() ; pit != parts.end() ; pit++) {
		(*pit)->reparent(NULL);
		delete *pit;
	}

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


void DFFGeometry::mirrorUVHorizontal()
{
	int numCoords = uvSetCount*vertexCount;

	for (int i = 0 ; i < numCoords ; i++) {
		uvCoordSets[i*2] = 1.0f - uvCoordSets[i*2];
	}

	/*int singleFloats = numFloats%4;
	Vec4SF vec1;
	vec1.f[0] = 1.0f;
	vec1.f[1] = 1.0f;
	vec1.f[2] = 1.0f;
	vec1.f[3] = 1.0f;

	for (int i = 0 ; i < numFloats; i += 4) {
		v4sf val;
		memcpy(&val, uvCoordSets+i, 16);
		v4sf* dest = (v4sf*) (uvCoordSets+i);
		*dest = vec1.v - val;
	}

	for (int i = 0 ; i < singleFloats ; i++) {
		float* ptr = uvCoordSets+(numFloats-(singleFloats-i));
		*ptr = 1.0f-*ptr;
	}*/
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
			material->reparent(NULL);
			materials.erase(it);
			return;
		}
	}
}


void DFFGeometry::removeMaterials()
{
	MaterialIterator it;

	for (it = materials.begin() ; it != materials.end() ; it++) {
		(*it)->reparent(NULL);
	}

	materials.clear();
}


void DFFGeometry::removePart(DFFGeometryPart* part)
{
	PartIterator it;

	for (it = parts.begin() ; it != parts.end() ; it++) {
		if (*it == part) {
			parts.erase(it);
			part->reparent(NULL);
			return;
		}
	}
}


void DFFGeometry::removeParts()
{
	PartIterator it;

	for (it = parts.begin() ; it != parts.end() ; it++) {
		(*it)->reparent(NULL);
	}

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


void DFFGeometry::reparent(DFFMesh* mesh)
{
	if (this->mesh  &&  mesh) {
		throw DFFException("Attempt to reparent a DFFGeometry which still has a parent! Remove it from "
				"it's old DFFMesh parent first.", __FILE__, __LINE__);
	}

	this->mesh = mesh;
}
