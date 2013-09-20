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

#ifndef COLLISIONMESH_H_
#define COLLISIONMESH_H_

#include <gta/config.h>
#include <gtaformats/col/COLModel.h>
#include "../ResourceCache.h"


class CollisionMesh
{
public:
	CollisionMesh(uint32_t numVertices, uint32_t numFaces, float* vertices, uint32_t* indices)
			: numVertices(numVertices), numFaces(numFaces), vertices(vertices), indices(indices)
			{}
	CollisionMesh(const COLModel& model);
	~CollisionMesh() { delete[] vertices; delete[] indices; }

	uint32_t getVertexCount() const { return numVertices; }
	uint32_t getFaceCount() const { return numFaces; }
	uint32_t getIndexCount() const { return numFaces*3; }
	float* getVertices() { return vertices; }
	const float* getVertices() const { return vertices; }
	uint32_t* getIndices() { return indices; }
	const uint32_t* getIndices() const { return indices; }

	cachesize_t getCacheSize() const;

private:
	uint32_t numVertices;
	uint32_t numFaces;

	float* vertices;
	uint32_t* indices;
};

#endif /* COLLISIONMESH_H_ */
