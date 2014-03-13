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

#include "Vehicle.h"
#include "../../Engine.h"
#include "../../resource/mesh/ManagedMeshPointer.h"
#include "../../resource/texture/ManagedTextureSource.h"
#include "../../resource/collision/ManagedCollisionShapePointer.h"
#include "../../resource/physics/ManagedPhysicsPointer.h"



Vehicle::Vehicle(const CString& modelName, const Matrix4& mm)
		: streamingDist(1000.0f), mass(100.0f), mm(mm)
{
	Engine* engine = Engine::getInstance();

	meshPtr = new ManagedMeshPointer(modelName);
	texSrc = new ManagedTextureSource(modelName);
	colPtr = new ManagedCollisionShapePointer(modelName);
	smeshPtr = NULL;
	physicsPtr = new ManagedPhysicsPointer(modelName);

	btRigidBody::btRigidBodyConstructionInfo info(mass, this, NULL);
	rb = new btRigidBody(info);
}


void Vehicle::getBoundingSphere(Vector3& center, float& radius)
{
	const float* a = mm.toArray();
	center = mm * boundingSphereCenter;
	radius = boundingSphereRadius;
}
