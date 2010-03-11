/*
 * DffModel.cpp
 *
 *  Created on: 04.01.2010
 *      Author: alemariusnexus
 */

#include "DFFMesh.h"


DFFMesh::~DFFMesh() {
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


void DFFMesh::mirrorYZ()
{
	for (int32_t i = 0 ; i < geometryCount ; i++) {
		geometries[i]->mirrorYZ();
	}

	for (int32_t i = 0 ; i < frameCount ; i++) {
		frames[i]->mirrorYZ();
	}
}


int32_t DFFMesh::indexOf(DFFFrame* frame)
{
	for (int32_t i = 0 ; i < frameCount ; i++) {
		if (frames[i] == frame) {
			return i;
		}
	}

	return -1;
}
