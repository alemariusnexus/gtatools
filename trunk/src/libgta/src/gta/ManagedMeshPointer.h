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
 */

#ifndef MANAGEDMESHPOINTER_H_
#define MANAGEDMESHPOINTER_H_

#include "config.h"
#include "MeshPointer.h"
#include <gtaformats/config.h>
#include "Engine.h"


class ManagedMeshPointer : public MeshPointer {
public:
	ManagedMeshPointer(hash_t meshHash) : meshHash(meshHash) {}
	ManagedMeshPointer(const ManagedMeshPointer& other) : meshHash(other.meshHash) {}
	hash_t getMeshHash() const { return meshHash; }
	void setMeshHash(hash_t hash) { meshHash = hash; }
	virtual Mesh* operator*();
	virtual MeshPointer* clone() const { return new ManagedMeshPointer(*this); }

private:
	hash_t meshHash;
};

#endif /* MANAGEDMESHPOINTER_H_ */
