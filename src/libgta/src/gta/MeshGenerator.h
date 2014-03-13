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

#ifndef MESHGENERATOR_H_
#define MESHGENERATOR_H_

#include <gta/config.h>
#include <nxcommon/math/Vector3.h>
#include "resource/mesh/Mesh.h"


class MeshGenerator {
public:
	void createBox(float*& vertices, int& vertexCount, uint32_t*& indices, int& indexCount,
			const Vector3& min, const Vector3& max);
	Mesh* createBox(const Vector3& min, const Vector3& max);
    void createSphere(float*& vertices, float*& normals, int& vertexCount, uint32_t*& indices, int& indexCount,
    		float radius, int slices, int stacks, const Vector3& offset = Vector3::Zero);
    Mesh* createSphere(float radius, int slices, int stacks, const Vector3& offset = Vector3::Zero);
};

#endif /* MESHGENERATOR_H_ */
