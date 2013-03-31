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

#ifndef RIGIDBODYSCENEOBJECT_H_
#define RIGIDBODYSCENEOBJECT_H_

#include <btBulletDynamicsCommon.h>
#include "SceneObject.h"
#include "../../resource/collision/CollisionShapePointer.h"



class RigidBodySceneObject : public virtual SceneObject, public btMotionState
{
public:
	virtual void getWorldTransform(btTransform& trans) const;
	virtual void setWorldTransform(const btTransform& trans);
	virtual const btRigidBody* getRigidBody() const = 0;
	virtual btRigidBody* getRigidBody() = 0;
	virtual CollisionShapePointer* getCollisionShapePointer() = 0;
	virtual float getMass() const = 0;
	virtual void setMass(float m) = 0;
};

#endif /* RIGIDBODYSCENEOBJECT_H_ */
