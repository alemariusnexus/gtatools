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
#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>



Vehicle::Vehicle(const CString& modelName, const Matrix4& mm)
		: rb(NULL), streamingDist(1000.0f), mass(1400.0f), mm(mm), force(0.0f), steeringAngle(0.0f)
{
	Engine* engine = Engine::getInstance();

	meshPtr = new ManagedMeshPointer(modelName);
	texSrc = new ManagedTextureSource(modelName);
	colPtr = new ManagedCollisionShapePointer(modelName);
	smeshPtr = NULL;
	physicsPtr = new ManagedPhysicsPointer(modelName);

	createWheel("wheel_lf_dummy");
	createWheel("wheel_rf_dummy");
	createWheel("wheel_lb_dummy");
	createWheel("wheel_rb_dummy");
	/*btRigidBody::btRigidBodyConstructionInfo info(mass, this, NULL);
	rb = new btRigidBody(info);*/
}


void Vehicle::createWheel(const CString& dummyName)
{
	WheelInfo wheel;
	wheel.dummyName = dummyName;

	MeshClump* clump = **meshPtr;

	MeshFrame* frame = clump->getRootFrame()->getChildByName(dummyName, true);
	Matrix4 wheelMat = frame->getAbsoluteModelMatrix();
	wheel.restPosition = wheelMat * Vector3::Zero;

	Mesh* wheelMesh = clump->getMeshByName("wheel");
	float* bounds = wheelMesh->getBounds();

	wheel.radius = bounds[3];

	GLuint dataBuffer = wheelMesh->getDataBuffer();
	glBindBuffer(GL_ARRAY_BUFFER, dataBuffer);
	char* bufData = (char*) glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);

	int voffs = wheelMesh->getVertexOffset();
	int vstride = wheelMesh->getVertexStride();
	int vcount = wheelMesh->getVertexCount();

	wheel.radius = 0.0f;

	for (int i = 0 ; i < vcount ; i++) {
		Vector3 vertex = *((Vector3*) (bufData + voffs + i*vstride));

		float len = vertex.length();

		if (len > wheel.radius) {
			wheel.radius = len;
		}
	}

	glUnmapBuffer(GL_ARRAY_BUFFER);

	printf("Wheel radius: %f\n", wheel.radius);

	wheel.maxSuspensionCompression = 0.1f;
	wheel.maxSuspensionRelaxation = 0.25f;
	//wheel.maxSuspensionCompression = 0.25f;
	//wheel.maxSuspensionRelaxation = 0.1f;
	wheel.suspensionStiffness = 1.2f;
	wheel.suspensionDamping = 0.19f;

	wheels.push_back(wheel);
}


void Vehicle::getBoundingSphere(Vector3& center, float& radius)
{
	const float* a = mm.toArray();
	center = mm * boundingSphereCenter;
	radius = boundingSphereRadius;
}


void Vehicle::getWorldTransform(btTransform& trans) const
{
	RigidBodySceneObject::getWorldTransform(trans);

#if 1
	btVector3 btComOffset(centerOfMassOffset);
	btComOffset = trans.getBasis() * btComOffset;

	trans = btTransform(trans.getRotation(), trans.getOrigin() - btComOffset);
#endif
}


void Vehicle::setWorldTransform(const btTransform& trans)
{
#if 1
	btVector3 btComOffset(centerOfMassOffset);
	btComOffset = trans.getBasis() * btComOffset;

	btTransform comTrans(trans.getRotation(), trans.getOrigin() + btComOffset);
#else
	btTransform comTrans(trans);
#endif

	// +com

	RigidBodySceneObject::setWorldTransform(comTrans);

}


void Vehicle::streamIn(btDiscreteDynamicsWorld* physWorld)
{
	delete rb;

	btCollisionShape* actualShape = physicsPtr->get(true);

	//centerOfMassOffset = Vector3(0.0f, 0.0f, -0.25f);
	centerOfMassOffset = Vector3(0.0f, 0.0f, 0.0f);

	btCompoundShape* shiftShape = new btCompoundShape;
	shiftShape->addChildShape(btTransform(btMatrix3x3::getIdentity(), centerOfMassOffset),
			actualShape);

	btVector3 inertia(0.0f, 0.0f, 0.0f);
	shiftShape->calculateLocalInertia(mass, inertia);

	btRigidBody::btRigidBodyConstructionInfo info(mass, this, shiftShape, inertia);
	rb = new btRigidBody(info);
	rb->setRestitution(0.2f);
	rb->setCcdMotionThreshold(0.00001f);
	rb->setCcdSweptSphereRadius(5.0f);
	rb->setDamping(0.1f, 0.0f);

	physWorld->addRigidBody(rb);
}


void Vehicle::streamOut(btDiscreteDynamicsWorld* physWorld)
{
	rb->setCollisionShape(NULL);
	physicsPtr->release();
	physWorld->removeRigidBody(rb);
}


void Vehicle::updatePhysics(btDiscreteDynamicsWorld* physWorld, uint64_t timePassed)
{
	//rb->applyForce(btVector3(0.0f, force, 0.0f), btVector3(0.0f, 0.0f, 0.0f));

#if 1
	forward = Vector3::UnitY;
	right = Vector3::UnitX;
	up = Vector3::UnitZ;

	basis = mm.getUpperLeft();

	forwardWS = basis * forward;
	rightWS = basis * right;
	upWS = basis * up;

	forwardWS.normalize();
	rightWS.normalize();
	upWS.normalize();

	for (WheelInfo& wheel : wheels) {
		updateWheel(wheel);
	}

	for (WheelInfo& wheel : wheels) {
		bool hasHit = castWheelRay(physWorld, wheel);
	}

	// Update suspension

#if 1
	float gravity = rb->getTotalForce().length();

	for (WheelInfo& wheel : wheels) {
		updateWheelSuspension(wheel, timePassed / 1000.0f);
	}

	float combinedForce = 0.0f;

	for (WheelInfo& wheel : wheels) {
		combinedForce += wheel.suspensionForce;
	}

	printf("Gravity: %.3f\n", rb->getGravity().length() / rb->getInvMass());

	printf("Combined suspension force: %.3fN\n", combinedForce);
#endif

	rb->applyForce(forwardWS*force, Vector3::Zero);
#endif

	rb->activate();
}


void Vehicle::setForce(float val)
{
	force = val;
}


void Vehicle::setSteeringAngle(float steering)
{
	steeringAngle = steering;
}


void Vehicle::updateWheel(WheelInfo& wheel)
{
	wheel.restPositionWS = mm * wheel.restPosition;

	if (wheel.dummyName == "wheel_lf_dummy") {
		//printf("transformMat: %f\n", wheel.currentSuspensionOffset);
	}
	wheel.transformMat = Matrix4::translation(-up * wheel.currentSuspensionOffset) * Matrix4::rotationZ(steeringAngle);
}


bool Vehicle::castWheelRay(btDiscreteDynamicsWorld* physWorld, WheelInfo& wheel)
{
	bool lbWheel = (wheel.dummyName == "wheel_lb_dummy");

	btVector3 rayStart = wheel.restPositionWS;
	btVector3 rayEnd = rayStart - upWS*wheel.radius;

	btDynamicsWorld::ClosestRayResultCallback cb(rayStart, rayEnd);
	cb.m_flags = btTriangleRaycastCallback::kF_UseSubSimplexConvexCastRaytest;

	physWorld->rayTest(rayStart, rayEnd, cb);

	bool hasHit = false;

	/*if (lbWheel) {
		printf("Casting ray (%.2f, %.2f, %.2f) -> (%.2f, %.2f, %.2f)\n", rayStart.getX(), rayStart.getY(), rayStart.getZ(),
				rayEnd.getX(), rayEnd.getY(), rayEnd.getZ());
	}*/

	float r;
	float num;

	hasHit = IntersectRayPlane(rayStart, rayEnd-rayStart, Vector3::UnitZ, Vector3(0.0f, 0.0f, 200.0f), r, num);

	if (r > 1.0f) {
		hasHit = false;
	}

	//if (cb.hasHit()) {
	if (hasHit) {
		//printf("Now it's hit: %f!\n", r);

		/*const btRigidBody* colBody = btRigidBody::upcast(cb.m_collisionObject);

		if (lbWheel) {
			printf("Vehicle %p has hit object %p\n", rb, colBody);
		}*/

		wheel.contactNormalWS = Vector3::UnitZ;
		wheel.contactPointWS = rayStart + (rayEnd-rayStart)*r;
		wheel.contactDist = (rayEnd-rayStart).length() * r;

		/*if (colBody  &&  colBody->hasContactResponse()) {
			wheel.contactNormalWS = cb.m_hitNormalWorld.normalized();
			wheel.contactPointWS = cb.m_hitPointWorld;
			wheel.contactDist = cb.m_closestHitFraction * (rayEnd - rayStart).length();

			hasHit = true;
		}*/
	}

	if (hasHit) {
		wheel.inContact = true;
		wheel.currentSuspensionOffset = wheel.contactDist - wheel.radius;

		if (wheel.currentSuspensionOffset < -wheel.maxSuspensionCompression) {
			wheel.currentSuspensionOffset = -wheel.maxSuspensionCompression;
		}

		if (lbWheel) {
			//printf("Suspension Offset: %f\n", wheel.currentSuspensionOffset);
		}

		/*if (wheel.dummyName == "wheel_lf_dummy") {
			printf("soffs: %f\n", wheel.currentSuspensionOffset);
		}*/

		// This is the cosine of the angle between contact normal and wheel direction
		float denom = wheel.contactNormalWS.dot(-upWS);

		// TODO: Check this
		wheel.contactPoint = wheel.contactPointWS - getPosition();

		Vector3 relVel = rb->getVelocityInLocalPoint(wheel.contactPoint);
		float projVel = wheel.contactNormalWS.dot(relVel);

		if (denom >= -0.1f) {
			// The contact normal points in a similar direction to the wheel direction. This is not normally
			// the case.
			wheel.clippedInvContactDotSuspension = 1.0f / 0.1f;
			wheel.suspensionVelocity = 0.0f;
		} else {
			wheel.clippedInvContactDotSuspension = -1.0f/denom;
			wheel.suspensionVelocity = projVel * wheel.clippedInvContactDotSuspension;
		}
	} else {
		if (wheel.dummyName == "wheel_lf_dummy") {
			//printf("NIC: %f\n", wheel.currentSuspensionOffset);
		}

		wheel.currentSuspensionOffset = 0.0f;
		wheel.clippedInvContactDotSuspension = 1.0f;
		wheel.suspensionVelocity = 0.0f;
		wheel.contactNormalWS = upWS;
		wheel.inContact = false;
	}

	//wheel.currentSuspensionOffset = wheel.maxSuspensionCompression;

	return hasHit;
}


void Vehicle::updateWheelSuspension(WheelInfo& wheel, float timeStep)
{
	bool lbWheel = (wheel.dummyName == "wheel_lb_dummy");

	if (wheel.inContact) {
		//printf("inContact\n");

		//printf("inv: %f\n", wheel.clippedInvContactDotSuspension);

		float force = -wheel.suspensionStiffness * wheel.currentSuspensionOffset * wheel.clippedInvContactDotSuspension;

		force -= wheel.suspensionDamping * wheel.suspensionVelocity;

		wheel.suspensionForce = force * mass;

		if (wheel.suspensionForce < 0.0f) {
			wheel.suspensionForce = 0.0f;
		}
	} else {
		wheel.suspensionForce = 0.0f;
	}

	if (lbWheel) {
		//printf("Force: %f, dt: %f\n", wheel.suspensionForce, timeStep);
	}

	//rb->applyForce(wheel.contactNormalWS * force, wheel.contactPoint);

	Vector3 impulse = wheel.contactNormalWS * wheel.suspensionForce * timeStep;

	/*if (lbWheel)
		printf("Impulse: %f, %f, %f\n", impulse.getX(), impulse.getY(), impulse.getZ());*/
	rb->applyImpulse(impulse, wheel.contactPoint);
}
