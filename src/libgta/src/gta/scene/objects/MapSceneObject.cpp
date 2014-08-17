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

#include "MapSceneObject.h"
#include "../../EngineException.h"



MapSceneObject::MapSceneObject()
		: enabled(true), defInfo(NULL), rb(NULL), visibleInstBeg(lodInsts.end()),
		  visibleInstEnd(lodInsts.end()), mass(0.0f), maxStreamingDist(0.0f), boundsValid(false), special(false)
{
	btRigidBody::btRigidBodyConstructionInfo info(0.0f, this, NULL);
	rb = new btRigidBody(info);
	rb->setRestitution(1.0f);
	rb->setFriction(1.0f);
}


MapSceneObject::MapSceneObject(const MapSceneObject& other)
		: enabled(other.enabled),
		  defInfo(other.defInfo ? new SceneObjectDefinitionInfo(*other.defInfo) : NULL), rb(NULL),
		  visibleInstBeg(lodInsts.end()), visibleInstEnd(lodInsts.end()), mass(other.mass),
		  maxStreamingDist(0.0f), boundsCenter(other.boundsCenter),
		  boundsRadius(other.boundsRadius), boundsValid(other.boundsValid), special(other.special)
{
	btRigidBody::btRigidBodyConstructionInfo info(mass, this, NULL);
	rb = new btRigidBody(info);

	for (LODInstanceMapIterator it = other.lodInsts.begin() ; it != other.lodInsts.end() ; it++) {
		lodInsts.insert(new MapSceneObjectLODInstance(**it));
	}
}


void MapSceneObject::addLODInstance(MapSceneObjectLODInstance* inst)
{
	if (inst->getSceneObject()) {
		throw EngineException("Attempt to add a LOD instance to a MapSceneObject that is already associated!",
				__FILE__, __LINE__);
	}

	boundsValid = false;

	inst->obj = this;
	lodInsts.insert(inst);
	inst->updateModelMatrix();

	if (inst->getStreamingDistance() > maxStreamingDist)
		maxStreamingDist = inst->getStreamingDistance();
}


void MapSceneObject::setMass(float m)
{
	/*btCollisionShape* shape = getCollisionShapePointer()->get();
	btVector3 inertia;
	shape->calculateLocalInertia(m, inertia);
	rb->setMassProps(m, inertia);
	mass = m;*/
}


void MapSceneObject::setModelMatrix(const Matrix4& matrix)
{
	mm = matrix;

	for (LODInstanceMapIterator it = lodInsts.begin() ; it != lodInsts.end() ; it++) {
		MapSceneObjectLODInstance* inst = *it;
		inst->updateModelMatrix();
	}

	btTransform trans;
	getWorldTransform(trans);
	rb->setWorldTransform(trans);
	rb->setInterpolationWorldTransform(trans);

	boundsValid = false;
}


void MapSceneObject::updateRenderingDistance(float dist, float sdMultiplier)
{
	for (LODInstanceMapIterator it = lodInsts.begin() ; it != lodInsts.end() ; it++) {
		MapSceneObjectLODInstance* inst = *it;

		if (inst->getStreamingDistance()*sdMultiplier >= dist) {
			if (visibleInstBeg == lodInsts.end()  ||  inst->getStreamingDistance() < (*visibleInstBeg)->getStreamingDistance()) {
				visibleInstBeg = it;

				for (	visibleInstEnd = visibleInstBeg ;
						visibleInstEnd != lodInsts.end()
								&&  (*visibleInstEnd)->getStreamingDistance()
									== (*visibleInstBeg)->getStreamingDistance()
						; visibleInstEnd++);
			}

			return;
		}
	}

	CString distStr("");

	for (LODInstanceMapIterator it = lodInsts.begin() ; it != lodInsts.end() ; it++) {
		MapSceneObjectLODInstance* inst = *it;
		char str[16];
		sprintf(str, "%.2f", inst->getStreamingDistance());

		if (distStr.length() != 0)
			distStr.append(CString(", "));

		distStr.append(CString(str));
	}

	char* errmsg = new char[256 + distStr.length()];
	sprintf(errmsg, "MapSceneObject::updateRenderingDistance() called with invalid distance: %.2f. Distance "
			"multiplier: %.2f. LOD instance distances: %s", dist, sdMultiplier, distStr.get());
	EngineException ex(errmsg, __FILE__, __LINE__);
	delete[] errmsg;
	throw ex;
}


void MapSceneObject::resetRenderingDistance()
{
	visibleInstBeg = lodInsts.end();
	visibleInstEnd = lodInsts.end();
}


void MapSceneObject::calculateBounds()
{
	if (boundsValid)
		return;

	const float* a = mm.toArray();
	boundsCenter = Vector3(a[12], a[13], a[14]);
	boundsRadius = 0.0f;

	if (!lodInsts.empty()) {
		MapSceneObjectLODInstance* lodInst = getLODInstance();

		MeshPointer* meshPtr = lodInst->getDefinition()->getMeshPointer();
		MeshClump* clump = meshPtr->get(true);

		if (clump) {
			clump->getBoundingSphere(boundsCenter, boundsRadius);
			boundsCenter = mm * Vector4(boundsCenter);
		}

		meshPtr->release();
	}

	Vector3 boxMin = Vector3::Zero;
	Vector3 boxMax = Vector3::Zero;

	CollisionShapePointer* colPtr = getCollisionShapePointer();

	if (colPtr) {
		CollisionModel* model = colPtr->get(true);

		if (model) {
			model->getBoundingSphere(colBoundingSphereCenter, colBoundingSphereRadius);
			model->getBoundingBox(boxMin, boxMax);
		}

		colPtr->release();
	}

	colBoundingBoxMin = mm * boxMin;
	colBoundingBoxExtX = (mm * Vector3(boxMax.getX(), boxMin.getY(), boxMin.getZ())) - colBoundingBoxMin;
	colBoundingBoxExtY = (mm * Vector3(boxMin.getX(), boxMax.getY(), boxMin.getZ())) - colBoundingBoxMin;
	colBoundingBoxExtZ = (mm * Vector3(boxMin.getX(), boxMin.getY(), boxMax.getZ())) - colBoundingBoxMin;

	colBoundingSphereCenter = mm * colBoundingSphereCenter;

	boundsValid = true;
}


void MapSceneObject::getBoundingSphere(Vector3& center, float& radius)
{
	calculateBounds();
	center = boundsCenter;
	radius = boundsRadius;
}


void MapSceneObject::getCollisionBoundingSphere(Vector3& center, float& radius)
{
	calculateBounds();
	center = colBoundingSphereCenter;
	radius = colBoundingSphereRadius;
}


void MapSceneObject::getCollisionBoundingBox(Vector3& min, Vector3& extX, Vector3& extY, Vector3& extZ)
{
	calculateBounds();
	min = colBoundingBoxMin;
	extX = colBoundingBoxExtX;
	extY = colBoundingBoxExtY;
	extZ = colBoundingBoxExtZ;
}


bool MapSceneObject::getLODInstances(LODInstanceMapIterator& beg, LODInstanceMapIterator& end, size_t depth)
{
	float lastDist = -1.0f;

	int i = 0;
	for (beg = lodInsts.begin() ; beg != lodInsts.end()  &&  i < depth ; beg++) {
		MapSceneObjectLODInstance* inst = *beg;

		if (inst->getStreamingDistance() != lastDist) {
			i++;
			lastDist = inst->getStreamingDistance();
		}
	}

	if (beg == lodInsts.end()) {
		end = beg;
		return false;
	}

	float dist = (*beg)->getStreamingDistance();
	for (end = beg ; end != lodInsts.end()  &&  (*end)->getStreamingDistance() == dist ; end++);

	return true;
}


bool MapSceneObject::hasAlphaTransparency() const
{
	if (visibleInstBeg != lodInsts.end()) {
		bool alpha = false;
		for (LODInstanceMapIterator it = visibleInstBeg ; it != visibleInstEnd  &&  !alpha ; it++)
			alpha = (*it)->hasAlphaTransparency();
		return alpha;
	} else {
		return getLODInstance()->hasAlphaTransparency();
	}
}


void MapSceneObject::lockRigidBodyCollisionShape(bool locked)
{
	CollisionShapePointer* colPtr = getCollisionShapePointer();

	CollisionModel* model = colPtr->get(true);
}

