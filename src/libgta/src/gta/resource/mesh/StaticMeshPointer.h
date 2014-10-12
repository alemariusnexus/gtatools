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

#ifndef STATICMESHPOINTER_H_
#define STATICMESHPOINTER_H_

#include <gta/config.h>
#include "MeshPointer.h"
#include "MeshClump.h"
#include <memory>

using std::shared_ptr;


class StaticMeshPointer : public MeshPointer {
public:
	StaticMeshPointer(MeshClump* mesh) : mesh(mesh) {}
	StaticMeshPointer(const StaticMeshPointer& other) : mesh(other.mesh) {}
	virtual MeshClump* get(bool lock = false) { return mesh; }
	virtual MeshPointer* clone() const { return new StaticMeshPointer(*this); }

private:
	MeshClump* mesh;
};

#endif /* STATICMESHPOINTER_H_ */
