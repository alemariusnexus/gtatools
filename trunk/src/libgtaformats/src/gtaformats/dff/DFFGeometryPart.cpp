/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#include "DFFGeometryPart.h"
#include "DFFGeometry.h"
#include "DFFException.h"
#include <cstring>


DFFGeometryPart::DFFGeometryPart(uint32_t ic, uint32_t* indices)
		: indexCount(ic), indices(indices), material(NULL), geometry(NULL)
{

}


DFFGeometryPart::DFFGeometryPart(const DFFGeometryPart& other)
		: indexCount(other.indexCount), indices(new uint32_t[indexCount]),
		  material(new DFFMaterial(*other.material)), geometry(NULL)
{
	memcpy(indices, other.indices, 4*indexCount);
}


DFFGeometryPart::~DFFGeometryPart()
{
	delete[] indices;
	// We don't delete the material because it's part of the whole geometry. Possibly more geometry parts can
	// share one material.
}


void DFFGeometryPart::setMaterial(DFFMaterial* mat)
{
	if (!geometry) {
		throw DFFException("DFFGeometryPart needs to have a DFFGeometry parent for setMaterial().",
				__FILE__, __LINE__);
	}
	if (geometry->indexOf(mat) == -1) {
		throw DFFException("Attempt to assign a material to a DFFGeometryPart which is not owned by it's "
				"DFFGeometry.", __FILE__, __LINE__);
	}

	material = mat;
}


void DFFGeometryPart::reparent(DFFGeometry* geom)
{
	if (geom  &&  geometry) {
		throw DFFException("Attempt to reparent a DFFGeometryPart which still has a parent! Remove it from "
				"it's old DFFGeometry parent first.", __FILE__, __LINE__);
	}

	geometry = geom;
}
