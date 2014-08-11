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

#ifndef VEHICLE_H_
#define VEHICLE_H_

#include "../parts/RigidBodySceneObject.h"
#include "../parts/VisualSceneObject.h"
#include "../../resource/mesh/MeshPointer.h"
#include "../../resource/texture/TextureSource.h"
#include "../../resource/collision/CollisionShapePointer.h"
#include "../../resource/physics/PhysicsPointer.h"
#include "../../scene/StreamingManager.h"
#include <vector>

using std::vector;



class Vehicle : public virtual VisualSceneObject, public virtual RigidBodySceneObject
{
public:
	struct WheelInfo
	{
		CString dummyName;
		Vector3 restPosition;
		Vector3 restPositionWS;
		float radius;
		float maxSuspensionCompression;
		float maxSuspensionRelaxation;
		float currentSuspensionOffset;
		float suspensionStiffness;
		float suspensionDamping;
		Vector3 contactPoint;
		Vector3 contactPointWS;
		Vector3 contactNormalWS;
		float contactDist;
		float suspensionVelocity;
		float clippedInvContactDotSuspension;
		float suspensionForce;
		bool inContact;
		Matrix4 transformMat;
	};

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

	virtual void getWorldTransform(btTransform& trans) const;
	virtual void setWorldTransform(const btTransform& trans);

	void streamIn(btDiscreteDynamicsWorld* physWorld);
	void streamOut(btDiscreteDynamicsWorld* physWorld);
	void updatePhysics(btDiscreteDynamicsWorld* physWorld, uint64_t timePassed);

	Vector3 getCenterOfMassOffset() const { return centerOfMassOffset; }

	void setForce(float val);
	void setSteeringAngle(float steering);

	float getSteeringAngle() const { return steeringAngle; }

	vector<WheelInfo>::iterator getWheelBegin() { return wheels.begin(); }
	vector<WheelInfo>::const_iterator getWheelBegin() const { return wheels.begin(); }

	vector<WheelInfo>::iterator getWheelEnd() { return wheels.end(); }
	vector<WheelInfo>::const_iterator getWheelEnd() const { return wheels.end(); }

private:
	void createWheel(const CString& dummyName);
	void updateWheel(WheelInfo& wheel);
	bool castWheelRay(btDiscreteDynamicsWorld* physWorld, WheelInfo& wheel);
	void updateWheelSuspension(WheelInfo& wheel, float timeStep);

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

	Vector3 centerOfMassOffset;
	float force;
	float steeringAngle;

	vector<WheelInfo> wheels;

	Vector3 forward, forwardWS;
	Vector3 right, rightWS;
	Vector3 up, upWS;

	Matrix3 basis;
};

#endif /* VEHICLE_H_ */
