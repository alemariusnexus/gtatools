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

#ifndef COLMESHCONVERTER_H_
#define COLMESHCONVERTER_H_

#include <gta/config.h>
#include <gtaformats/col/COLModel.h>
#include "../mesh/Mesh.h"


class COLMeshConverter {
public:
	Mesh* convert(const float* vertices, unsigned int vertexCount, const COLFace* faces, unsigned int faceCount);
	Mesh* convert(const COLModel& model);
	Mesh* convert(const COLSphere& sphere);
	Mesh* convert(const COLBox& box);
	void convertVertexModel(const float* inVertices, unsigned int inVertexCount, const COLFace* inFaces,
			unsigned int inFaceCount, unsigned int& outVertexCount, float*& outVertices, uint8_t*& outColors,
			uint32_t*& outIndices, unsigned int& outIndexCount);

private:
	void getMaterialColors(uint8_t mat, uint8_t& r, uint8_t& g, uint8_t& b);
};

#endif /* COLMESHCONVERTER_H_ */
