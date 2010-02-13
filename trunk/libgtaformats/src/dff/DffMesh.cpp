/*
 * DffModel.cpp
 *
 *  Created on: 04.01.2010
 *      Author: alemariusnexus
 */

#include "DffMesh.h"


DffMesh::~DffMesh() {
	if (frames) {
		for (int32_t i = 0 ; i < frameCount ; i++) {
			delete frames[i];
		}

		delete[] frames;
	}

	for (int32_t i = 0 ; i < geometryCount ; i++) {
		delete geometries[i];
	}

	delete[] geometries;
}


void DffMesh::mirrorYZ()
{
	for (int32_t i = 0 ; i < geometryCount ; i++) {
		geometries[i]->mirrorYZ();
	}

	for (int32_t i = 0 ; i < frameCount ; i++) {
		frames[i]->mirrorYZ();
	}
}
