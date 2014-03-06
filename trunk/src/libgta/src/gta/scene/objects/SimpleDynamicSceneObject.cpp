#include "SimpleDynamicSceneObject.h"




SimpleDynamicSceneObject::SimpleDynamicSceneObject(MeshPointer* mptr, TextureSource* tsrc, CollisionShapePointer* cptr,
		ShadowMeshPointer* smptr, PhysicsPointer* pptr, float sd, float mass, const Matrix4& mm)
		: streamingDist(sd), mass(mass), mm(mm), meshPtr(mptr), texSrc(tsrc), colPtr(cptr), smeshPtr(smptr), physicsPtr(pptr)
{
	btRigidBody::btRigidBodyConstructionInfo info(mass, this, NULL);
	rb = new btRigidBody(info);
}


SimpleDynamicSceneObject::SimpleDynamicSceneObject(const SimpleDynamicSceneObject& other)
		: streamingDist(other.streamingDist), mass(other.mass), mm(other.mm), meshPtr(other.meshPtr), texSrc(other.texSrc),
		  colPtr(other.colPtr), smeshPtr(other.smeshPtr), physicsPtr(other.physicsPtr)
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
