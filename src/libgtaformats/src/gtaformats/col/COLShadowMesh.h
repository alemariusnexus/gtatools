/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef COLSHADOWMESH_H_
#define COLSHADOWMESH_H_

#include <gtaformats/config.h>
#include "COLFace.h"
#include <vector>


using std::vector;



class COLShadowMesh {
	friend class COLLoader;

public:
	COLShadowMesh(uint32_t numVertices, float* vertices)
			: numVertices(numVertices), vertices(vertices) {}
	COLShadowMesh(const COLShadowMesh& other);
	~COLShadowMesh();
	uint32_t getVertexCount() const { return numVertices; }
	size_t getFaceCount() const { return faces.size(); }
	const float* getVertices() const { return vertices; }
	float* getVertices() { return vertices; }
	const vector<COLFace>& getFaces() const { return faces; }
	vector<COLFace>& getFaces() { return faces; }
	void setVertices(uint32_t num, float* vertices) { numVertices = num; this->vertices = vertices; }
	void setFaces(const vector<COLFace>& faces) { this->faces = faces; }

private:
	COLShadowMesh() {}

private:
	uint32_t numVertices;
	float* vertices;
	vector<COLFace> faces;
};

#endif /* COLSHADOWMESH_H_ */
