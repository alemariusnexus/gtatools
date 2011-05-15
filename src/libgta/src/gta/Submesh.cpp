/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "Submesh.h"
#include "GLException.h"



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


Submesh::~Submesh()
{
	glDeleteBuffers(1, &indexBuffer);
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
