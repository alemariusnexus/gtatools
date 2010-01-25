/*
 * DffModel.cpp
 *
 *  Created on: 04.01.2010
 *      Author: alemariusnexus
 */

#include "DffMesh.h"

DffMesh::DffMesh() {

}

DffMesh::~DffMesh() {
	if (bounds)
		delete bounds;
	if (faceIndices)
		delete[] faceIndices;
	if (submeshFaceIndexCounts)
		delete[] submeshFaceIndexCounts;
	if (vertexColors)
		delete[] vertexColors;
	if (vertexNormals)
		delete[] vertexNormals;
	if (vertexPositions)
		delete[] vertexPositions;
}

void DffMesh::scale(float factor) {
	for (int i = 0 ; i < vertexCount ; i++) {
		vertexPositions[i*3] *= factor;
		vertexPositions[i*3 + 1] *= factor;
		vertexPositions[i*3 + 2] *= factor;
	}
}
