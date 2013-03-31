/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#ifndef MANAGEDMESHPOINTER_H_
#define MANAGEDMESHPOINTER_H_

#include <gta/config.h>
#include "MeshPointer.h"
#include <gtaformats/config.h>
#include <gtaformats/util/CString.h>
#include "../../Engine.h"
#include "../ResourceCache.h"


class ManagedMeshPointer : public MeshPointer {
public:
	ManagedMeshPointer(const CString& meshName)
			: pointer(Engine::getInstance()->getMeshCache()->getEntryPointer(meshName)) {}
	ManagedMeshPointer(const ManagedMeshPointer& other) : pointer(other.pointer) {}
	virtual MeshClump* get(bool lock = false);
	virtual void lock(bool lock = true);
	virtual MeshPointer* clone() const { return new ManagedMeshPointer(*this); }
	virtual void release();

private:
	Engine::StringResourceCache::Pointer pointer;
};

#endif /* MANAGEDMESHPOINTER_H_ */
