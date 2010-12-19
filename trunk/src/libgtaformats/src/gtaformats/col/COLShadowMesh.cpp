/*
 * COLShadowMesh.cpp
 *
 *  Created on: 31.10.2010
 *      Author: alemariusnexus
 */

#include "COLShadowMesh.h"



COLShadowMesh::COLShadowMesh(const COLShadowMesh& other)
		: numVertices(other.numVertices), numFaces(other.numFaces), vertices(new float[numVertices*3]),
		  faces(new COLFace[numFaces])
{
	memcpy(vertices, other.vertices, numVertices*12);
	memcpy(faces, other.faces, numFaces*sizeof(COLFace));
}
