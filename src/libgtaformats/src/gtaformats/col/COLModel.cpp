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

#include "COLModel.h"



COLModel::COLModel(const COLModel& other)
		: bounds(other.bounds), numSpheres(other.numSpheres), numBoxes(other.numBoxes),
		  numFaces(other.numFaces), flags(other.flags), numVertices(other.numVertices),
		  numFaceGroups(other.numFaceGroups), spheres(new COLSphere[numSpheres]),
		  boxes(new COLBox[numSpheres]), vertices(new float[numVertices*3]), faces(new COLFace[numFaces]),
		  faceGroups(new COLFaceGroup[numFaceGroups]),
		  shadowMesh(other.shadowMesh ? new COLShadowMesh(*other.shadowMesh) : NULL)
{
	memcpy(name, other.name, 20);
	memcpy(spheres, other.spheres, numSpheres*sizeof(COLSphere));
	memcpy(boxes, other.boxes, numBoxes*sizeof(COLBox));
	memcpy(vertices, other.vertices, numVertices*12);
	memcpy(faces, other.faces, numFaces*sizeof(COLFace));
	memcpy(faceGroups, other.faceGroups, numFaceGroups*sizeof(COLFaceGroup));
}


COLModel::COLModel()
		: numSpheres(0), numBoxes(0), numFaces(0), flags(0), numVertices(0), numFaceGroups(0), spheres(NULL),
		  boxes(NULL), vertices(NULL), faces(NULL), faceGroups(NULL), shadowMesh(NULL)
{
}
