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

#ifndef MANAGEDSHADOWMESHPOINTER_H_
#define MANAGEDSHADOWMESHPOINTER_H_

#include "ShadowMeshPointer.h"
#include "../../Engine.h"



class ManagedShadowMeshPointer : public ShadowMeshPointer
{
public:
	ManagedShadowMeshPointer(const CString& meshName)
			: pointer(Engine::getInstance()->getCollisionMeshCache()->getEntryPointer(meshName)) {}
	ManagedShadowMeshPointer(const ManagedShadowMeshPointer& other) : pointer(other.pointer) {}
	virtual ShadowMeshPointer* clone() const { return new ManagedShadowMeshPointer(*this); }
	virtual ShadowMesh* get(bool lock = false);
	virtual void release() { pointer.release(); }

private:
	Engine::StringResourceCache::Pointer pointer;
};

#endif /* MANAGEDSHADOWMESHPOINTER_H_ */
