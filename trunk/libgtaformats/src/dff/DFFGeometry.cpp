/*
 * DFFGeometry.cpp
 *
 *  Created on: 12.02.2010
 *      Author: alemariusnexus
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
	for (int32_t i = 0 ; i < vertexCount ; i++) {
		float y = vertices[(i*3) + 1];
		float z = vertices[(i*3) + 2];
		vertices[(i*3) + 1] = z;
		vertices[(i*3) + 2] = y;

		if (normals != NULL) {
			float ny = normals[(i*3) + 1];
			float nz = normals[(i*3) + 2];
			normals[(i*3) + 1] = nz;
			normals[(i*3) + 2] = ny;
		}
	}
}
