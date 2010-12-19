/*
 * COLModel.cpp
 *
 *  Created on: 31.10.2010
 *      Author: alemariusnexus
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
