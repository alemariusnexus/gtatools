#ifndef VEHICLE_H_
#define VEHICLE_H_

#include "../parts/RigidBodySceneObject.h"
#include "../parts/VisualSceneObject.h"
#include "../../resource/mesh/MeshPointer.h"
#include "../../resource/texture/TextureSource.h"
#include "../../resource/collision/CollisionShapePointer.h"
#include "../../resource/physics/PhysicsPointer.h"
#include "../../scene/StreamingManager.h"



class Vehicle : public virtual VisualSceneObject, public virtual RigidBodySceneObject
{
public:
	Vehicle(const CString& modelName, const Matrix4& mm);
	virtual SceneObject* clone() const { return new Vehicle(*this); }
	virtual typeflags_t getTypeFlags() const { return TypeFlagVisual | TypeFlagRigidBody; }
	virtual float getStreamingDistance() const { return streamingDist; }
	virtual uint32_t getStreamingBuckets() const { return StreamingManager::VisibleBucket | StreamingManager::PhysicsBucket; }
	virtual Matrix4& getModelMatrix() { return mm; }
	virtual const Matrix4& getModelMatrix() const { return mm; }
	virtual void setModelMatrix(const Matrix4& matrix) { mm = matrix; }

	virtual bool hasAlphaTransparency() const { return true; }
	virtual void getBoundingSphere(Vector3& center, float& radius);
	virtual ShadowMeshPointer* getCurrentShadowMeshPointer() { return smeshPtr; }

	virtual void updateRenderingDistance(float dist, float sdMultiplier) {}
	virtual void resetRenderingDistance() {}

	virtual CollisionShapePointer* getCollisionShapePointer() { return colPtr; }
	virtual float getMass() const { return mass; }
	virtual void setMass(float m) {}
	virtual void getCollisionBoundingSphere(Vector3& center, float& radius) { getBoundingSphere(center, radius); }
	virtual void getCollisionBoundingBox(Vector3& min, Vector3& extX, Vector3& extY, Vector3& extZ) {}

	void setBoundingSphere(const Vector3& center, float radius) { boundingSphereCenter = center; boundingSphereRadius = radius; }

	MeshPointer* getMeshPointer() { return meshPtr; }
	TextureSource* getTextureSource() { return texSrc; }
	PhysicsPointer* getPhysicsPointer() { return physicsPtr; }

	btRigidBody* getRigidBody() { return rb; }
	void setRigidBody(btRigidBody* rb) { this->rb = rb; }

private:
	btRigidBody* rb;
	float streamingDist;
	float mass;
	Matrix4 mm;

	Vector3 boundingSphereCenter;
	float boundingSphereRadius;
	Vector3 boundingBoxMin;
	Vector3 boundingBoxMax;

	MeshPointer* meshPtr;
	TextureSource* texSrc;
	CollisionShapePointer* colPtr;
	ShadowMeshPointer* smeshPtr;
	PhysicsPointer* physicsPtr;
};

#endif /* VEHICLE_H_ */
