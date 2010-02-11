/*
 * DffModel.cpp
 *
 *  Created on: 04.01.2010
 *      Author: alemariusnexus
 */

#include "DffMesh.h"

DffMesh::DffMesh()
		: vertexPositions(NULL), vertexNormals(NULL), vertexColors(NULL), textureCoordinateSets(NULL),
		  faceIndices(NULL), submeshFaceIndexCounts(NULL), bounds(NULL), materials(NULL), frames(NULL)
{

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
	if (textureCoordinateSets)
		delete[] textureCoordinateSets;

	if (materials) {
		for (int32_t i = 0 ; i < materialCount ; i++) {
			delete materials[i];
		}

		delete[] materials;
	}

	if (frames) {
		for (int32_t i = 0 ; i < frameCount ; i++) {
			delete frames[i];
		}

		delete[] frames;
	}
}

void DffMesh::scale(float factor) {
	for (int i = 0 ; i < vertexCount ; i++) {
		vertexPositions[i*3] *= factor;
		vertexPositions[i*3 + 1] *= factor;
		vertexPositions[i*3 + 2] *= factor;
	}
}
