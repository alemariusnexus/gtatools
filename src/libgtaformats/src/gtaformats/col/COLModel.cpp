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

#include "COLModel.h"



COLModel::COLModel(const COLModel& other)
		: name(other.name), bounds(other.bounds), flags(other.flags), numVertices(other.numVertices),
		  spheres(other.spheres),
		  boxes(other.boxes), vertices(new float[numVertices*3]), faces(other.faces),
		  faceGroups(other.faceGroups),
		  shadowMesh(other.shadowMesh ? new COLShadowMesh(*other.shadowMesh) : NULL)
{
	memcpy(vertices, other.vertices, numVertices*12);
}


COLModel::COLModel()
		: flags(0), numVertices(0), vertices(NULL), shadowMesh(NULL)
{
}


COLModel::~COLModel()
{
	delete[] vertices;
	delete shadowMesh;
}
