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

#ifndef MANAGEDPHYSICSPOINTER_H_
#define MANAGEDPHYSICSPOINTER_H_

#include "PhysicsPointer.h"
#include "../../Engine.h"


class ManagedPhysicsPointer : public PhysicsPointer
{
public:
	ManagedPhysicsPointer(const CString& name)
			: ptr(Engine::getInstance()->getPhysicsCache()->getEntryPointer(name)) {}
	ManagedPhysicsPointer(const ManagedPhysicsPointer& other) : ptr(other.ptr) {}
	virtual PhysicsPointer* clone() const { return new ManagedPhysicsPointer(*this); }
	virtual btCollisionShape* get(bool lock = false);

private:
	Engine::StringResourceCache::Pointer ptr;
};

#endif /* MANAGEDPHYSICSPOINTER_H_ */
