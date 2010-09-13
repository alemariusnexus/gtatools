/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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


DFFMesh::DFFMesh(const DFFMesh& other)
{
	ConstGeometryIterator git;
	for (git = other.getGeometryBegin() ; git != other.getGeometryEnd() ; git++) {
		geometries.push_back(new DFFGeometry(**git));
	}

	ConstFrameIterator fit;
	for (fit = other.getFrameBegin() ; fit != other.getFrameEnd() ; fit++) {
		frames.push_back(new DFFFrame(**fit));
	}
}


DFFMesh::~DFFMesh() {
	removeFrames();
	removeGeometries();
}


void DFFMesh::mirrorYZ()
{
	GeometryIterator git;
	for (git = geometries.begin() ; git != geometries.end() ; git++) {
		(*git)->mirrorYZ();
	}

	FrameIterator fit;
	for (fit = frames.begin() ; fit != frames.end() ; fit++) {
		(*fit)->mirrorYZ();
	}
}


void DFFMesh::scale(float x, float y, float z)
{
	GeometryIterator git;
	for (git = geometries.begin() ; git != geometries.end() ; git++) {
		(*git)->scale(x, y, z);
	}

	FrameIterator fit;
	for (fit = frames.begin() ; fit != frames.end() ; fit++) {
		(*fit)->scale(x, y, z);
	}
}


int32_t DFFMesh::indexOf(const DFFFrame* frame) const
{
	ConstFrameIterator it;
	int i = 0;

	for (it = frames.begin() ; it != frames.end() ; it++, i++) {
		if (*it == frame) {
			return i;
		}
	}

	return -1;
}


const DFFGeometry* DFFMesh::getGeometry(const char* name) const
{
	ConstGeometryIterator it;

	for (it = geometries.begin() ; it != geometries.end() ; it++) {
		if (strcmp((*it)->getAssociatedFrame()->getName(), name) == 0) {
			return *it;
		}
	}

	return NULL;
}


DFFGeometry* DFFMesh::getGeometry(const char* name)
{
	GeometryIterator it;

	for (it = geometries.begin() ; it != geometries.end() ; it++) {
		if (strcmp((*it)->getAssociatedFrame()->getName(), name) == 0) {
			return *it;
		}
	}

	return NULL;
}


const DFFFrame* DFFMesh::getFrame(const char* name) const
{
	ConstFrameIterator it;

	for (it = frames.begin() ; it != frames.end() ; it++) {
		if (strcmp((*it)->getName(), name) == 0) {
			return *it;
		}
	}

	return NULL;
}


DFFFrame* DFFMesh::getFrame(const char* name)
{
	FrameIterator it;

	for (it = frames.begin() ; it != frames.end() ; it++) {
		if (strcmp((*it)->getName(), name) == 0) {
			return *it;
		}
	}

	return NULL;
}


void DFFMesh::removeFrame(DFFFrame* frame)
{
	FrameIterator it;

	for (it = frames.begin() ; it != frames.end() ; it++) {
		if (*it == frame) {
			delete *it;
			frames.erase(it);
		}
	}
}


void DFFMesh::removeGeometry(DFFGeometry* geom)
{
	GeometryIterator it;

	for (it = geometries.begin() ; it != geometries.end() ; it++) {
		if (*it == geom) {
			delete *it;
			geometries.erase(it);
		}
	}
}


void DFFMesh::removeFrames()
{
	FrameIterator it;

	for (it = frames.begin() ; it != frames.end() ; it++) {
		delete *it;
	}

	frames.clear();
}


void DFFMesh::removeGeometries()
{
	GeometryIterator it;

	for (it = geometries.begin() ; it != geometries.end() ; it++) {
		delete *it;
	}

	geometries.clear();
}
