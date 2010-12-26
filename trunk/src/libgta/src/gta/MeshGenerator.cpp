/*
	Copyright 2010 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MeshGenerator.h"
#include <cmath>


void MeshGenerator::createBox(float*& vertices, int& vertexCount, int32_t*& indices, int& indexCount,
		const Vector3& min, const Vector3& max)
{
    float minx = min.getX();
	float miny = min.getY();
	float minz = min.getZ();
	float maxx = max.getX();
	float maxy = max.getY();
	float maxz = max.getZ();

	vertices = new float[8*3];
	float localVertices[] = {
			minx, miny, minz,
			maxx, miny, minz,
			maxx, maxy, minz,
			minx, maxy, minz,
			minx, miny, maxz,
			maxx, miny, maxz,
			maxx, maxy, maxz,
			minx, maxy, maxz
	};
	memcpy(vertices, localVertices, 8*3*4);
	vertexCount = 8*3;

	indices = new int32_t[6*6];
	float localIndices[] = {
			0, 1, 2, 2, 3, 0,	// Front
			4, 5, 6, 6, 7, 4,	// Back
			3, 2, 6, 6, 7, 3,	// Top
			4, 5, 1, 1, 0, 4,	// Bottom
			4, 0, 3, 3, 7, 4,	// Left
			1, 5, 6, 6, 2, 1	// Right
	};
	memcpy(indices, localIndices, 6*6*4);
	indexCount = 6*6;
}


void MeshGenerator::createSphere(float*& vertices, int& vertexCount, int32_t*& indices, int& indexCount,
		float radius, int slices, int stacks)
{
    int numVertices = (stacks-1)*slices+2;
    vertexCount = numVertices;
	vertices = new float[numVertices*3];
	vertices[0] = 0.0f;
	vertices[1] = 0.0f;
	vertices[2] = radius;

	float stackStepAngle = PI/stacks;
	float sliceStepAngle = (2*PI)/slices;

	int vertexOffset = 3;
	int sv = 1; // Start Vertex

	for (int i = 1 ; i < stacks ; i++) {
		float stackAngle = stackStepAngle*i;
		float stackSin = sin(stackAngle);
		float stackCos = cos(stackAngle);

		for (int j = 0 ; j < slices ; j++) {
			float sliceAngle = sliceStepAngle*j;
			vertices[vertexOffset] = cos(sliceAngle)*radius*stackSin;
			vertices[vertexOffset+1] = sin(sliceAngle)*radius*stackSin;
			vertices[vertexOffset+2] = stackCos*radius;
			vertexOffset += 3;
		}
	}

	vertices[vertexOffset] = 0.0f;
	vertices[vertexOffset+1] = 0.0f;
	vertices[vertexOffset+2] = -radius;


	int numIndices = (2*slices*3) + (2*3*slices*(stacks-2));
	indexCount = numIndices;
	int indexOffset = 0;

	indices = new int32_t[numIndices];

	for (int i = 0 ; i < slices-1 ; i++) {
		indices[indexOffset++] = 0;
		indices[indexOffset++] = i+1;
		indices[indexOffset++] = i+2;
	}
	indices[indexOffset++] = 0;
	indices[indexOffset++] = slices;
	indices[indexOffset++] = 1;

	for (int i = 0 ; i < stacks-2 ; i++) {
		for (int j = 0 ; j < slices-1 ; j++) {
			indices[indexOffset++] = sv + j + 1;
			indices[indexOffset++] = sv + j;
			indices[indexOffset++] = sv + slices + j;
			indices[indexOffset++] = sv + slices + j;
			indices[indexOffset++] = sv + slices + j + 1;
			indices[indexOffset++] = sv + j + 1;
		}

		indices[indexOffset++] = sv;
		indices[indexOffset++] = sv + slices-1;
		indices[indexOffset++] = sv + slices + slices-1;
		indices[indexOffset++] = sv + slices + slices-1;
		indices[indexOffset++] = sv + slices;
		indices[indexOffset++] = sv;

		sv += slices;
	}

	for (int i = 0 ; i < slices-1 ; i++) {
		indices[indexOffset++] = sv+slices;
		indices[indexOffset++] = sv+i+1;
		indices[indexOffset++] = sv+i;
	}
	indices[indexOffset++] = sv + slices;
	indices[indexOffset++] = sv;
	indices[indexOffset++] = sv + slices - 1;
}
