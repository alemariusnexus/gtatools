/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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
#include "../../GLException.h"
#include "../../Engine.h"



Submesh::Submesh(Mesh* mesh, int indexCount, uint32_t* indices)
		: mesh(mesh), material(NULL), indexCount(indexCount), indexOffset(0)
{
	this->indices = indices;
	mesh->addSubmesh(this);
}


Submesh::Submesh(Mesh* mesh, const DFFGeometryPart& part)
		: mesh(mesh), material(NULL), indexCount(part.getIndexCount()), indexOffset(0)
{
	this->indices = part.getIndices();
	mesh->addSubmesh(this);

	DFFMaterial* mat = part.getMaterial();

	if (mat) {
		int32_t index = part.getGeometry()->indexOf(mat);
		material = mesh->getMaterial(index);
	}
}


Submesh::~Submesh()
{
}


void Submesh::setLinked(GLuint indexOffset)
{
	this->indexOffset = indexOffset;
	indices = NULL;
}


void Submesh::setMaterial(Material* mat)
{
	material = mat;
}

