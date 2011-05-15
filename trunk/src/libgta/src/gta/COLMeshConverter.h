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

#ifndef COLMESHCONVERTER_H_
#define COLMESHCONVERTER_H_

#include <gta/config.h>
#include <gtaformats/col/COLModel.h>
#include "Mesh.h"


class COLMeshConverter {
public:
	Mesh* convert(const float* vertices, int vertexCount, const COLFace* faces, int faceCount);
	Mesh* convert(const COLModel& model);
	Mesh* convert(const COLSphere& sphere);
	Mesh* convert(const COLBox& box);
	void convertVertexModel(const float* inVertices, int32_t inVertexCount, const COLFace* inFaces,
			int32_t inFaceCount, int& outVertexCount, float*& outVertices, uint8_t*& outColors,
			int32_t*& outIndices, int32_t& outIndexCount);

private:
	void getMaterialColors(uint8_t mat, uint8_t& r, uint8_t& g, uint8_t& b);
};

#endif /* COLMESHCONVERTER_H_ */
