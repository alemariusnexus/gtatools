/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of gtatools-test.

	gtatools-test is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-test is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-test.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "VehicleController.h"
#include <gta/scene/objects/SimpleDynamicSceneObject.h>
#include <gta/scene/objects/MapSceneObject.h>
#include <gta/scene/objects/MapSceneObjectLODInstance.h>
#include <gta/resource/mesh/ManagedMeshPointer.h>
#include <gta/resource/mesh/StaticMeshPointer.h>
#include <gta/resource/texture/ManagedTextureSource.h>
#include <gta/resource/texture/StaticTextureSource.h>
#include <gta/resource/texture/NullTextureSource.h>
#include <gta/resource/collision/StaticCollisionShapePointer.h>
#include <gta/resource/collision/ManagedCollisionShapePointer.h>
#include <gta/resource/collision/CollisionModelBulletConverter.h>
#include <gta/resource/physics/StaticPhysicsPointer.h>
#include <gta/resource/physics/ManagedPhysicsPointer.h>
#include <gta/StaticMapItemDefinition.h>
#include <BulletDynamics/Vehicle/btRaycastVehicle.h>
#include <BulletCollision/NarrowPhaseCollision/btRaycastCallback.h>



#define FLOOR_HALF_WIDTH 1000.0f
#define FLOOR_TEX_COORD_SCALE 25.0f





VehicleController::VehicleController()
		: engine(Engine::getInstance()), scene(engine->getScene()), forceActive(false), torqueActive(false),
		  numUpdates(0), scc(new SphericalCameraController), lastMouseX(-1), lastMouseY(-1)
{
}


MapSceneObject* VehicleController::generateFloor()
{
	float vertices[] = {
			-FLOOR_HALF_WIDTH, -FLOOR_HALF_WIDTH, 0.0f,
			-FLOOR_HALF_WIDTH, FLOOR_HALF_WIDTH, 0.0f,
			FLOOR_HALF_WIDTH, -FLOOR_HALF_WIDTH, 0.0f,
			FLOOR_HALF_WIDTH, FLOOR_HALF_WIDTH, 0.0f
	};

	float texCoords[] = {
			0.0f, 0.0f,
			0.0f, FLOOR_TEX_COORD_SCALE,
			FLOOR_TEX_COORD_SCALE, 0.0f,
			FLOOR_TEX_COORD_SCALE, FLOOR_TEX_COORD_SCALE
	};

	float normals[] = {
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f
	};

	uint32_t indices[] = {
			0, 1, 2, 3
	};

	MeshClump* clump = new MeshClump;

	Mesh* mesh = new Mesh(4, VertexFormatTriangleStrips, MeshNormals | MeshTriangleStrips | MeshTexCoords,
			vertices, normals, texCoords, NULL, NULL, NULL);
	mesh->setBounds(0.0f, 0.0f, 0.0f, FLOOR_HALF_WIDTH * 2.0f);
	Submesh* sm = new Submesh(mesh, 4, indices);
	mesh->link();

	Material* mat = new Material;
	mat->setTextured(true);
	mat->setTexture("sjmalley");
	mat->setColor(255, 255, 255, 255);
	sm->setMaterial(mat);
	mesh->addMaterial(mat);

	MeshFrame* rootFrame = new MeshFrame;
	clump->setRootFrame(rootFrame);

	mesh->setFrame(rootFrame);

	clump->addMesh(mesh);

	CollisionBox* colBox = new CollisionBox(Vector3(-FLOOR_HALF_WIDTH, -FLOOR_HALF_WIDTH, -0.5f),
			Vector3(FLOOR_HALF_WIDTH, FLOOR_HALF_WIDTH, 0.0f));
	CollisionModel* colModel = new CollisionModel;
	colModel->addBox(colBox);

	CollisionModelBulletConverter conv;

	btCollisionShape* colShape = conv.convert(colModel, NULL);
	colShape->setMargin(1.0f);

	//btBoxShape* colShape = new btBoxShape(btVector3(FLOOR_HALF_WIDTH, FLOOR_HALF_WIDTH, 0.05f));

	StaticMeshPointer* meshPtr = new StaticMeshPointer(clump);
	TextureSource* texSrc = new ManagedTextureSource("ce_bankalley1");
	CollisionShapePointer* colPtr = new StaticCollisionShapePointer(colModel);
	PhysicsPointer* physPtr = new StaticPhysicsPointer(colShape);

	StaticMapItemDefinition* def = new StaticMapItemDefinition(meshPtr, texSrc, colPtr, NULL, physPtr, FLOOR_HALF_WIDTH * 3.0f, 0);

	MapSceneObjectLODInstance* lodInst = new MapSceneObjectLODInstance(def);

	MapSceneObject* obj = new MapSceneObject;
	obj->addLODInstance(lodInst);

	obj->getRigidBody()->setUserPointer((void*) 0x12345678);

	return obj;
}


void VehicleController::init()
{
	printf("VHC Init\n");

	MapSceneObject* floor = generateFloor();
	floor->setModelMatrix(Matrix4::translation(0.0f, 0.0f, 200.0f));

	scene->addSceneObject(floor);

	veh = new Vehicle("infernus", Matrix4::translation(0.0f, 0.0f, 205.0f));

	/*ManagedMeshPointer* meshPtr = new ManagedMeshPointer("infernus");
	ManagedTextureSource* texSrc = new ManagedTextureSource("infernus");
	ManagedCollisionShapePointer* colPtr = new ManagedCollisionShapePointer("infernus");
	ManagedPhysicsPointer* physPtr = new ManagedPhysicsPointer("infernus");

	btCollisionShape* actualShape = physPtr->get(false);

	Vector3 comOffset(0.0f, 0.0f, -0.25f);

	btCompoundShape* shiftShape = new btCompoundShape;
	shiftShape->addChildShape(btTransform(btMatrix3x3::getIdentity(), btVector3(comOffset.getX(), comOffset.getX(), comOffset.getZ())),
			actualShape);

	StaticPhysicsPointer* actualPhysPtr = new StaticPhysicsPointer(shiftShape);

	chassis = new SimpleDynamicSceneObject(meshPtr, texSrc, colPtr, NULL, actualPhysPtr, 1500000.0f, 1400.0f,
			Matrix4::translation(0.0f, 0.0f, 205.0f));
	chassis->setCenterOfMassOffset(comOffset);
	chassis->setBoundingSphere(Vector3::Zero, 10.0f);
	btRigidBody* rb = chassis->getRigidBody();
	rb->setDamping(0.1f, 0.0f);

	scene->addSceneObject(chassis);*/

	scene->addSceneObject(veh);

	scc->setRadius(10.0f);
	scc->setAzimuth(M_PI_2 + M_PI);
	scc->setInclination(M_PI_2 - 0.25f);
}


uint64_t VehicleController::update(uint64_t timePassed)
{
#if 0
	btDiscreteDynamicsWorld* physWorld = scene->getPhysicsWorld();

	printf("Num: %u\n", (unsigned int) physWorld->getNumCollisionObjects());

#if 1
	Vector3 rayStart(-2.006414f, 93.087769f, 200.258331f);
	Vector3 rayEnd(-2.006469f, 93.086464f, 199.886978f);

	rayStart = rayStart + (rayStart-rayEnd)*50.0f;
	rayEnd = rayStart + (rayEnd-rayStart)*2.0f;

	printf("Vector3 rayStart(%ff, %ff, %ff);\n", rayStart.getX(), rayStart.getY(), rayStart.getZ());
	printf("Vector3 rayEnd(%ff, %ff, %ff);\n", rayEnd.getX(), rayEnd.getY(), rayEnd.getZ());
#else
	Vector3 rayStart(-2.006414f, 93.087769f, 200.258331f);
	Vector3 rayEnd(-2.006469f, 93.086464f, 195.886978f);
#endif

	btDynamicsWorld::ClosestRayResultCallback cb(rayStart, rayEnd);
	cb.m_flags = btTriangleRaycastCallback::kF_UseSubSimplexConvexCastRaytest;

	physWorld->rayTest(rayStart, rayEnd, cb);

	if (cb.hasHit()) {
		printf("Has hit!\n");
	} else {
		Vector3 altEnd = rayStart + (rayEnd-rayStart)*2.0f;

		btDynamicsWorld::ClosestRayResultCallback acb(rayStart, altEnd);
		acb.m_flags = btTriangleRaycastCallback::kF_UseSubSimplexConvexCastRaytest;

		physWorld->rayTest(rayStart, altEnd, acb);

		if (acb.hasHit()) {
			printf("Should have hit!\n");
		} else {
			printf("Hasn't hit at all!\n");
		}
	}

	exit(0);
#endif

	if (timePassed != 0) {
		if (forceActive) {
			veh->setForce(50000.0f);
		} else {
			veh->setForce(0.0f);
		}

		veh->updatePhysics(scene->getPhysicsWorld(), timePassed);
	}

	/*btRigidBody* rb = chassis->getRigidBody();

	if (forceActive) {
		rb->applyForce(btVector3(0.0f, 50000.0f, 0.0f), btVector3(0.0f, 0.0f, 0.0f));
	}
	if (torqueActive) {
		rb->applyTorque(btVector3(10000.0f, 0.0f, 0.0f));
	}

	rb->activate();

	btVector3 avel = rb->getAngularVelocity();

	btVector3 torque = rb->getTotalTorque();
	//printf("Linear Speed: %.2f, Angular Speed: %.2f\n", rb->getLinearVelocity().length(), avel.length());

	Matrix4 mm = chassis->getModelMatrix();
	const float* mma = mm.toArray();
	Vector3 pos = Vector3(mma[12], mma[13], mma[14]);

	scc->setCenter(pos);
	//printf("%.2f, %.2f, %.2f\n", pos.getX(), pos.getY(), pos.getZ());

	scc->apply();*/

	scc->setCenter(veh->getPosition());
	scc->apply();

	//Camera* cam = engine->getCamera();
	//cam->setPosition(pos + Vector3(0.0f, -5.0f, 2.5f));

	/*numUpdates++;

	if (numUpdates == 5) {
		btDiscreteDynamicsWorld* world = scene->getPhysicsWorld();

		btVector3 source(0.0f, 0.0f, 50.0f);
		btVector3 target(0.0f, -10.0f, 0.0f);
		btDiscreteDynamicsWorld::ClosestRayResultCallback res(source, target);
		world->rayTest(source, target, res);

		if (res.hasHit()) {
			btVector3 n = res.m_hitNormalWorld;
			printf("Hit normal: %.2f, %.2f, %.2f\n", n.x(), n.y(), n.z());
		} else {
			printf("Didn't hit!\n");
		}

		exit(0);
	}*/

	return timePassed;
}


void VehicleController::keyPressed(SDL_keysym evt)
{
	SDLKey k = evt.sym;

	btRigidBody* rb;

	if (veh) {
		rb = veh->getRigidBody();
	}

	if (k == SDLK_UP) {
		//printf("Applying force\n");
		forceActive = true;
	} else if (k == SDLK_DOWN) {
		torqueActive = true;
	} else if (k == SDLK_LEFT) {
		/*btVector3 inertia(0.0f, 0.0f, 0.0f);

		PhysicsPointer* pptr = chassis->getPhysicsPointer();
		if (pptr) {
			btCollisionShape* shape = pptr->get();
			shape->calculateLocalInertia(chassis->getMass(), inertia);
		}

		rb->setMassProps(chassis->getMass(), inertia);*/

		veh->setSteeringAngle(M_PI*0.25f);
	} else if (k == SDLK_RIGHT) {
		veh->setSteeringAngle(-M_PI*0.25f);
	} else if (k == SDLK_SPACE) {
		btTransform trf = rb->getWorldTransform();
		trf = trf * btTransform(btMatrix3x3::getIdentity(), btVector3(0.0f, 0.0f, 5.0f));
		rb->setWorldTransform(trf);
		//chassis->setModelMatrix(chassis->getModelMatrix() * Matrix4::translation(0.0f, 0.0f, 25.0f));
		rb->activate(true);
	}
}


void VehicleController::keyReleased(SDL_keysym evt)
{
	SDLKey k = evt.sym;

	if (k == SDLK_UP) {
		forceActive = false;
	} else if (k == SDLK_DOWN) {
		torqueActive = false;
	} else if (k == SDLK_LEFT) {
		veh->setSteeringAngle(0.0f);
	} else if (k == SDLK_RIGHT) {
		veh->setSteeringAngle(0.0f);
	}
}


void VehicleController::mouseButtonPressed(Uint8 button, int x, int y)
{
	if (button == SDL_BUTTON_LEFT) {
		lastMouseX = x;
		lastMouseY = y;
	} else if (button == SDL_BUTTON_WHEELUP) {
		scc->move(-0.5f);
	} else if (button == SDL_BUTTON_WHEELDOWN) {
		scc->move(0.5f);
	}
}


void VehicleController::mouseButtonDoubleClicked(int x, int y)
{

}


void VehicleController::mouseButtonReleased(Uint8 button, int x, int y)
{
	if (button == SDL_BUTTON_LEFT) {
		lastMouseX = -1;
		lastMouseY = -1;
	}
}


void VehicleController::mouseMotion(int x, int y)
{
	if (lastMouseX != -1) {
		float azimuth = (x - lastMouseX) * -0.0035f;
		float inclination = (y - lastMouseY) * -0.0035f;

		scc->rotateAzimuth(azimuth);
		scc->rotateInclination(inclination);

		lastMouseX = x;
		lastMouseY = y;
	}
}


