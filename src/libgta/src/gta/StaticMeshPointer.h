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

#ifndef STATICMESHPOINTER_H_
#define STATICMESHPOINTER_H_

#include "config.h"
#include "MeshPointer.h"
#include "Mesh.h"


class StaticMeshPointer : public MeshPointer {
public:
	StaticMeshPointer(Mesh* mesh) : mesh(mesh) {}
	StaticMeshPointer(const StaticMeshPointer& other) : mesh(other.mesh) {}
	virtual Mesh* operator*() { return mesh; }
	virtual MeshPointer* clone() const { return new StaticMeshPointer(*this); }

private:
	Mesh* mesh;
};

#endif /* STATICMESHPOINTER_H_ */
