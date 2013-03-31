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

#ifndef COLLISIONSHAPEPOINTER_H_
#define COLLISIONSHAPEPOINTER_H_

#include <btBulletDynamicsCommon.h>

class CollisionShapePointer {
public:
	virtual CollisionShapePointer* clone() const = 0;
	virtual btCollisionShape* get(bool lock = false) = 0;
	virtual void release() {}

	virtual btCollisionShape* operator*() { return get(); }
};

#endif /* COLLISIONSHAPEPOINTER_H_ */
