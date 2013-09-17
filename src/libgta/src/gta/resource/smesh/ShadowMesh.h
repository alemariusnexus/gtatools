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

#ifndef SHADOWMESH_H_
#define SHADOWMESH_H_

#include <gtaformats/col/COLShadowMesh.h>
#include <map>
#include <vector>

using std::map;
using std::vector;



class ShadowMesh
{
private:
	struct EdgeInfo
	{
		struct FaceInfo
		{
			uint32_t index;
			uint32_t additionalIndex;
		};
		vector<FaceInfo> faceIndices;
	};

	typedef map<uint32_t, map<uint32_t, EdgeInfo> > EdgeMap;
	typedef EdgeMap::iterator EdgeMapIterator;
	typedef EdgeMap::const_iterator ConstEdgeMapIterator;

public:
	ShadowMesh(uint32_t numVertices, uint32_t numFaces, float* vertices, uint32_t* indices);
	ShadowMesh(const COLShadowMesh& smesh);
	ShadowMesh(const ShadowMesh& other);
	~ShadowMesh();

private:
	void calculateMissingData();
	void decoupleEdges(EdgeMap& edges);

public:
	uint32_t numVertices;
	uint32_t numFaces;
	float* vertices;
	uint32_t* indices;

	float* faceNormals;
	uint32_t* adjacentFaces;
};

#endif /* SHADOWMESH_H_ */
