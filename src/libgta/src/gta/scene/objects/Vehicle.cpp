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
