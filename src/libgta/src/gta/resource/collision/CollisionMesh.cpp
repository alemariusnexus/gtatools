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

#include "CollisionMesh.h"



CollisionMesh::CollisionMesh(const COLModel& model)
		: numVertices(model.getVertexCount()), numFaces(model.getFaceCount()),
		  vertices(new float[numVertices*3]), indices(new uint32_t[numFaces*3])
{
	memcpy(vertices, model.getVertices(), numVertices*3*sizeof(float));

	const COLFace* faces = model.getFaceCount() != 0 ? &model.getFaces()[0] : NULL;

	for (size_t i = 0 ; i < numFaces ; i++) {
		const COLFace& face = faces[i];
		const uint32_t* faceIndices = face.getIndices();
		memcpy(indices+i*3, faceIndices, 3*sizeof(uint32_t));
	}
}


cachesize_t CollisionMesh::getCacheSize() const
{
	return sizeof(CollisionMesh) + numVertices*3*sizeof(float) + numFaces*3*sizeof(uint32_t);
}

