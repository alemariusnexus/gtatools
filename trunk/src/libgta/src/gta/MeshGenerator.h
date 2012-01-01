/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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
#include <gtaformats/util/math/Vector3.h>
#include "resource/mesh/Mesh.h"


class MeshGenerator {
public:
	void createBox(float*& vertices, int& vertexCount, uint32_t*& indices, int& indexCount,
			const Vector3& min, const Vector3& max);
	Mesh* createBox(const Vector3& min, const Vector3& max);
    void createSphere(float*& vertices, int& vertexCount, uint32_t*& indices, int& indexCount,
    		float radius, int slices, int stacks);
    Mesh* createSphere(float radius, int slices, int stacks);
};

#endif /* MESHGENERATOR_H_ */
