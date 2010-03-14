/*
	Copyright 2010 David Lerch

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


DFFGeometry::~DFFGeometry()
{
	if (vertexColors != NULL) {
		delete[] vertexColors;
	}
	if (uvCoordSets != NULL) {
		for (uint8_t i = 0 ; i < uvSetCount ; i++) {
			delete[] uvCoordSets[i];
		}

		delete[] uvCoordSets;
	}
	if (vertices != NULL) {
		delete[] vertices;
	}
	if (normals != NULL) {
		delete[] normals;
	}


	for (int32_t i = 0 ; i < materialCount ; i++) {
		delete materials[i];
	}

	delete[] materials;


	for (int32_t i = 0 ; i < partCount ; i++) {
		delete parts[i];
	}

	delete[] parts;
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


int32_t DFFGeometry::indexOf(DFFMaterial* mat) const
{
	for (int32_t i = 0 ; i < materialCount ; i++) {
		if (materials[i] == mat) {
			return i;
		}
	}

	return -1;
}


/*int32_t DFFGeometry::indexOf(DFFFrame* frame)
{
	for (int32_t i = 0 ; i < frameCount ; i++) {
		if (frames[i] == frame) {
			return i;
		}
	}

	return -1;
}*/
