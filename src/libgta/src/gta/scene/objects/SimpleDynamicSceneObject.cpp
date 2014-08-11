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

#include "SimpleDynamicSceneObject.h"




SimpleDynamicSceneObject::SimpleDynamicSceneObject(MeshPointer* mptr, TextureSource* tsrc, CollisionShapePointer* cptr,
		ShadowMeshPointer* smptr, PhysicsPointer* pptr, float sd, float mass, const Matrix4& mm)
		: streamingDist(sd), mass(mass), mm(mm), comOffset(Vector3::Zero), meshPtr(mptr), texSrc(tsrc), colPtr(cptr),
		  smeshPtr(smptr), physicsPtr(pptr)
{
	btRigidBody::btRigidBodyConstructionInfo info(mass, this, NULL);
	rb = new btRigidBody(info);
}


SimpleDynamicSceneObject::SimpleDynamicSceneObject(const SimpleDynamicSceneObject& other)
		: streamingDist(other.streamingDist), mass(other.mass), mm(other.mm), comOffset(other.comOffset),
		  meshPtr(other.meshPtr), texSrc(other.texSrc), colPtr(other.colPtr), smeshPtr(other.smeshPtr), physicsPtr(other.physicsPtr)
{
	btRigidBody::btRigidBodyConstructionInfo info(mass, this, NULL);
	rb = new btRigidBody(info);
}


void SimpleDynamicSceneObject::getBoundingSphere(Vector3& center, float& radius)
{
	const float* a = mm.toArray();
	center = mm * boundingSphereCenter;
	radius = boundingSphereRadius;
}


void SimpleDynamicSceneObject::setCenterOfMassOffset(const Vector3& comOffset)
{
	this->comOffset = comOffset;
}


void SimpleDynamicSceneObject::getWorldTransform(btTransform& trans) const
{
	RigidBodySceneObject::getWorldTransform(trans);

#if 1
	btVector3 btComOffset(comOffset.getX(), comOffset.getY(), comOffset.getZ());
	btComOffset = trans.getBasis() * btComOffset;

	trans = btTransform(trans.getRotation(), trans.getOrigin() - btComOffset);
#endif
}


void SimpleDynamicSceneObject::setWorldTransform(const btTransform& trans)
{
#if 1
	btVector3 btComOffset(comOffset.getX(), comOffset.getY(), comOffset.getZ());
	btComOffset = trans.getBasis() * btComOffset;

	btTransform comTrans(trans.getRotation(), trans.getOrigin() + btComOffset);
#else
	btTransform comTrans(trans);
#endif

	// +com

	RigidBodySceneObject::setWorldTransform(comTrans);

}
