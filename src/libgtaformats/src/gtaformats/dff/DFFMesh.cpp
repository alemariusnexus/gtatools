/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "DFFMesh.h"
#include <nxcommon/exception/OutOfBoundsException.h>
#include <cstring>


DFFMesh::DFFMesh(const DFFMesh& other)
{
	ConstGeometryIterator git;
	for (git = other.getGeometryBegin() ; git != other.getGeometryEnd() ; git++) {
		geometries.push_back(new DFFGeometry(**git));
	}
}


DFFMesh::~DFFMesh() {
	GeometryIterator it;
	for (it = geometries.begin() ; it != geometries.end() ; it++) {
		(*it)->reparent(NULL);
		delete *it;
	}

	delete integratedCOLModel;
}


void DFFMesh::mirrorUVHorizontal()
{
	GeometryIterator git;
	for (git = geometries.begin() ; git != geometries.end() ; git++) {
		(*git)->mirrorUVHorizontal();
	}
}


const DFFGeometry* DFFMesh::getGeometry(const CString& name) const
{
	ConstGeometryIterator it;

	for (it = geometries.begin() ; it != geometries.end() ; it++) {
		if ((*it)->getAssociatedFrame()->getName() == name) {
			return *it;
		}
	}

	return NULL;
}


DFFGeometry* DFFMesh::getGeometry(const CString& name)
{
	GeometryIterator it;

	for (it = geometries.begin() ; it != geometries.end() ; it++) {
		if ((*it)->getAssociatedFrame()->getName() == name) {
			return *it;
		}
	}

	return NULL;
}


void DFFMesh::removeGeometry(DFFGeometry* geom)
{
	GeometryIterator it;

	for (it = geometries.begin() ; it != geometries.end() ; it++) {
		if (*it == geom) {
			(*it)->reparent(NULL);
			geometries.erase(it);
		}
	}
}


void DFFMesh::removeGeometries()
{
	GeometryIterator it;

	for (it = geometries.begin() ; it != geometries.end() ; it++) {
		(*it)->reparent(NULL);
	}

	geometries.clear();
}


DFFGeometry* DFFMesh::getGeometry(uint32_t idx)
{
	if (idx >= geometries.size()) {
		throw OutOfBoundsException(idx, __FILE__, __LINE__);
	}

	return geometries[idx];
}


const DFFGeometry* DFFMesh::getGeometry(uint32_t idx) const
{
	if (idx >= geometries.size()) {
		throw OutOfBoundsException(idx, __FILE__, __LINE__);
	}

	return geometries[idx];
}
