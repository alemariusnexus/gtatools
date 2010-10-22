/*
 * Submesh.cpp
 *
 *  Created on: 21.10.2010
 *      Author: alemariusnexus
 */

#include "Submesh.h"



Submesh::Submesh(Mesh* mesh, int indexCount, int32_t* indices)
		: mesh(mesh), material(NULL), indexCount(indexCount)
{
	init(indices);
}


Submesh::Submesh(Mesh* mesh, const DFFGeometryPart& part)
		: mesh(mesh), material(NULL), indexCount(part.getIndexCount())
{
	DFFMaterial* mat = part.getMaterial();

	if (mat) {
		int32_t index = part.getGeometry()->indexOf(mat);
		material = mesh->getMaterial(index);
	}

	init(part.getIndices());
}


void Submesh::init(int32_t* indices)
{
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount*4, indices, GL_STATIC_DRAW);
}


void Submesh::setMaterial(Material* mat)
{
	material = mat;
}


void Submesh::bindIndexBuffer()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
}
