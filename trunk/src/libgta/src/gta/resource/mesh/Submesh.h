/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#ifndef SUBMESH_H_
#define SUBMESH_H_

#include <gta/config.h>
#include "../../gl.h"
#include "Mesh.h"
#include "Material.h"
#include <gtaformats/dff/DFFGeometryPart.h>



class Submesh {
public:
	Submesh(Mesh* mesh, int indexCount, uint32_t* indices);
	Submesh(Mesh* mesh, const DFFGeometryPart& part);
	~Submesh();
	Material* getMaterial() { return material; }
	void setMaterial(Material* mat);
	GLuint getIndexBuffer() const { return indexBuffer; }
	void bindIndexBuffer();
	int getIndexCount() const { return indexCount; }

	int guessSize() const { return indexCount*4 + sizeof(Submesh); }

private:
	void init(uint32_t* indices);

private:
	Mesh* mesh;
	Material* material;
	int indexCount;
	GLuint indexBuffer;
};

#endif /* SUBMESH_H_ */
