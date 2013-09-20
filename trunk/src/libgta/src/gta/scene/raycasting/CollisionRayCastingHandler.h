#ifndef COLLISIONRAYCASTINGHANDLER_H_
#define COLLISIONRAYCASTINGHANDLER_H_

#include "RayCastingHandler.h"
#include "../../resource/collision/CollisionModel.h"
#include "../parts/RigidBodySceneObject.h"
#include "../objects/MapSceneObject.h"



template <class ItType>
class CollisionRayCastingHandler : public RayCastingHandler
{
public:
	enum ObjectResultFlags
	{
		ObjectResultFlagSphere = 1 << 0,
		ObjectResultFlagBox = 1 << 1,
		ObjectResultFlagMesh = 1 << 2
	};

	struct ObjectResult
	{
		RigidBodySceneObject* obj;
		uint32_t flags;
		SphereResult sphereRes;
		BoxResult boxRes;
		MeshResult meshRes;
	};

	typedef list<ObjectResult> ResultList;
	typedef typename ResultList::iterator ResultIterator;
	typedef typename ResultList::const_iterator ConstResultIterator;

private:
	struct ObjectContext
	{
		RigidBodySceneObject* obj;
		CollisionModel* colModel;

		CollisionModel::SphereIterator sphereIt;
		CollisionModel::BoxIterator boxIt;
		CollisionModel::MeshIterator meshIt;

		uint32_t sphereIdx;
		uint32_t boxIdx;
		uint32_t meshIdx;

		CollisionModel::SphereIterator* sphereIts;
		CollisionModel::BoxIterator* boxIts;
		CollisionModel::MeshIterator* meshIts;
	};

public:
	virtual ~CollisionRayCastingHandler() {}

	void setSceneObjects(ItType beg, ItType end) { sobjBeg = beg; sobjEnd = end; }
	ResultIterator getResultBegin() { return results.begin(); }
	ResultIterator getResultEnd() { return results.end(); }

	virtual void startRayCasting(const Vector3& rayStart, const Vector3& rayDir, int flags);

	virtual bool nextObject(ObjectData& data, void*& userPtr);

	virtual bool loadObjectDetails(void* objUserPtr);

	virtual bool nextSphere(void* objUserPtr, SphereData& data, void*& userPtr);
	virtual bool nextBox(void* objUserPtr, BoxData& data, void*& userPtr);
	virtual bool nextMesh(void* objUserPtr, MeshData& data, void*& userPtr);

	virtual void sphereIntersectionReported(const SphereResult& res, void* objUserPtr, void* userPtr);
	virtual void boxIntersectionReported(const BoxResult& res, void* objUserPtr, void* userPtr);
	virtual void meshIntersectionReported(const MeshResult& res, void* objUserPtr, void* userPtr);

	virtual void finishObject(void* userPtr);

	virtual void finishRayCasting();

private:
	ItType sobjBeg, sobjEnd, sobjNext;
	ResultList results;

	int currentFlags;
};




template <class ItType>
void CollisionRayCastingHandler<ItType>::startRayCasting(const Vector3& rayStart, const Vector3& rayDir, int flags)
{
	sobjNext = sobjBeg;
	currentFlags = flags;
	results.clear();
}


template <class ItType>
bool CollisionRayCastingHandler<ItType>::nextObject(ObjectData& data, void*& userPtr)
{
	bool validObjectFound = false;

	RigidBodySceneObject* obj;
	CollisionShapePointer* colPtr;

	while (!validObjectFound) {
		if (sobjNext == sobjEnd)
			return false;

		SceneObject* sobj = *sobjNext++;

		if ((sobj->getTypeFlags() & SceneObject::TypeFlagRigidBody)  !=  0) {
			obj = dynamic_cast<RigidBodySceneObject*>(sobj);

			colPtr = obj->getCollisionShapePointer();

			if (colPtr) {
				validObjectFound = true;
			}
		}
	}

	ObjectContext* ctx = new ObjectContext;

	ctx->obj = obj;
	ctx->colModel = NULL;

	data.flags = ObjectDataFlagBoundingSphere | ObjectDataFlagBoundingBox;
	obj->getCollisionBoundingSphere(data.boundingSphereCenter, data.boundingSphereRadius);
	obj->getCollisionBoundingBox(data.boundingBoxMin, data.boundingBoxExtX, data.boundingBoxExtY, data.boundingBoxExtZ);
	data.modelMatrix = obj->getModelMatrix();

	userPtr = ctx;

	return true;
}


template <class ItType>
bool CollisionRayCastingHandler<ItType>::loadObjectDetails(void* objUserPtr)
{
	ObjectContext* ctx = (ObjectContext*) objUserPtr;

	CollisionShapePointer* colPtr = ctx->obj->getCollisionShapePointer();
	CollisionModel* model;

	model = colPtr->get(true);

	if (!model) {
		colPtr->release();
		return false;
	}

	ctx->colModel = model;

	ctx->sphereIt = model->getSphereBegin();
	ctx->boxIt = model->getBoxBegin();
	ctx->meshIt = model->getMeshBegin();

	ctx->sphereIdx = 0;
	ctx->boxIdx = 0;
	ctx->meshIdx = 0;

	ctx->sphereIts = new CollisionModel::SphereIterator[model->getSphereCount()];
	ctx->boxIts = new CollisionModel::BoxIterator[model->getBoxCount()];
	ctx->meshIts = new CollisionModel::MeshIterator[model->getMeshCount()];

	return true;
}


template <class ItType>
bool CollisionRayCastingHandler<ItType>::nextSphere(void* objUserPtr, SphereData& data, void*& userPtr)
{
	ObjectContext* ctx = (ObjectContext*) objUserPtr;

	CollisionModel* model = ctx->colModel;

	if (ctx->sphereIt == model->getSphereEnd())
		return false;

	CollisionSphere* sphere = *ctx->sphereIt;

	data.center = sphere->getCenter();
	data.radius = sphere->getRadius();

	ctx->sphereIts[ctx->sphereIdx] = ctx->sphereIt;

	userPtr = ctx->sphereIts + ctx->sphereIdx;

	ctx->sphereIt++;
	ctx->sphereIdx++;

	return true;
}


template <class ItType>
bool CollisionRayCastingHandler<ItType>::nextBox(void* objUserPtr, BoxData& data, void*& userPtr)
{
	ObjectContext* ctx = (ObjectContext*) objUserPtr;

	CollisionModel* model = ctx->colModel;

	if (ctx->boxIt == model->getBoxEnd())
		return false;

	CollisionBox* box = *ctx->boxIt;

	Vector3 min = box->getMinimum();
	Vector3 max = box->getMaximum();

	data.min = box->getMinimum();
	data.max = box->getMaximum();

	ctx->boxIts[ctx->boxIdx] = ctx->boxIt;

	userPtr = ctx->boxIts + ctx->boxIdx;

	ctx->boxIt++;
	ctx->boxIdx++;

	return true;
}


template <class ItType>
bool CollisionRayCastingHandler<ItType>::nextMesh(void* objUserPtr, MeshData& data, void*& userPtr)
{
	ObjectContext* ctx = (ObjectContext*) objUserPtr;

	CollisionModel* model = ctx->colModel;

	if (ctx->meshIt == model->getMeshEnd())
		return false;

	CollisionMesh* mesh = *ctx->meshIt;

	data.numVertices = mesh->getVertexCount();
	data.numFaces = mesh->getFaceCount();
	data.vertices = mesh->getVertices();
	data.indices = mesh->getIndices();

	ctx->meshIts[ctx->meshIdx] = ctx->meshIt;

	userPtr = ctx->meshIts + ctx->meshIdx;

	ctx->meshIt++;
	ctx->meshIdx++;

	return true;
}


template <class ItType>
void CollisionRayCastingHandler<ItType>::sphereIntersectionReported(const SphereResult& res, void* objUserPtr, void* userPtr)
{
	ObjectContext* ctx = (ObjectContext*) objUserPtr;
	CollisionModel::SphereIterator it = *((CollisionModel::SphereIterator*) userPtr);

	ObjectResult ores;
	ores.obj = ctx->obj;
	ores.flags = ObjectResultFlagSphere;
	ores.sphereRes = res;

	results.push_back(ores);
}


template <class ItType>
void CollisionRayCastingHandler<ItType>::boxIntersectionReported(const BoxResult& res, void* objUserPtr, void* userPtr)
{
	ObjectContext* ctx = (ObjectContext*) objUserPtr;
	CollisionModel::BoxIterator it = *((CollisionModel::BoxIterator*) userPtr);

	ObjectResult ores;
	ores.obj = ctx->obj;
	ores.flags = ObjectResultFlagBox;
	ores.boxRes = res;

	results.push_back(ores);
}


template <class ItType>
void CollisionRayCastingHandler<ItType>::meshIntersectionReported(const MeshResult& res, void* objUserPtr, void* userPtr)
{
	ObjectContext* ctx = (ObjectContext*) objUserPtr;
	CollisionModel::MeshIterator it = *((CollisionModel::MeshIterator*) userPtr);

	ObjectResult ores;
	ores.obj = ctx->obj;
	ores.flags = ObjectResultFlagMesh;
	ores.meshRes = res;

	results.push_back(ores);
}


template <class ItType>
void CollisionRayCastingHandler<ItType>::finishObject(void* userPtr)
{
	ObjectContext* ctx = (ObjectContext*) userPtr;

	if (ctx->colModel) {
		delete[] ctx->boxIts;
		delete[] ctx->sphereIts;
		delete[] ctx->meshIts;

		ctx->obj->getCollisionShapePointer()->release();
	}

	delete ctx;
}


template <class ItType>
void CollisionRayCastingHandler<ItType>::finishRayCasting()
{

}


template
class CollisionRayCastingHandler<list<RigidBodySceneObject*>::iterator>;

#endif /* COLLISIONRAYCASTINGHANDLER_H_ */
