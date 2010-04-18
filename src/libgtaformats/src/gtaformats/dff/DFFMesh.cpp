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

#include "DFFMesh.h"
#include <cstring>


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


void DFFMesh::scale(float x, float y, float z)
{
	for (int32_t i = 0 ; i < geometryCount ; i++) {
		geometries[i]->scale(x, y, z);
	}

	for (int32_t i = 0 ; i < frameCount ; i++) {
		frames[i]->scale(x, y, z);
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


DFFGeometry* DFFMesh::getGeometry(const char* name) const
{
	for (int32_t i = 0 ; i < geometryCount ; i++) {
		if (strcmp(geometries[i]->getAssociatedFrame()->getName(), name) == 0) {
			return geometries[i];
		}
	}

	return NULL;
}


DFFFrame* DFFMesh::getFrame(const char* name) const
{
	for (int32_t i = 0 ; i < frameCount ; i++) {
		if (strcmp(frames[i]->getName(), name) == 0) {
			return frames[i];
		}
	}

	return NULL;
}
