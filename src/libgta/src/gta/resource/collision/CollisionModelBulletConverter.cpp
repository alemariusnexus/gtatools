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

#include "CollisionModelBulletConverter.h"



btCollisionShape* CollisionModelBulletConverter::convert(const CollisionSphere* sphere, cachesize_t* cacheSize)
{
	if (cacheSize)
		*cacheSize = sizeof(btSphereShape);

	btSphereShape* shape = new btSphereShape(sphere->getRadius());
	return shape;
}


btCollisionShape* CollisionModelBulletConverter::convert(const CollisionBox* box, cachesize_t* cacheSize)
{
	if (cacheSize)
		*cacheSize = sizeof(btBoxShape);

	const Vector3& min = box->getMinimum();
	const Vector3& max = box->getMaximum();
	btBoxShape* shape = new btBoxShape(btVector3(
			(max.getX()-min.getX()) / 2.0f,
			(max.getY()-min.getY()) / 2.0f,
			(max.getZ()-min.getZ()) / 2.0f));
	return shape;
}


btCollisionShape* CollisionModelBulletConverter::convert(const float* vertices, int vertexCount, const uint32_t* indices,
		int indexCount, cachesize_t* cacheSize)
{
	if (cacheSize)
		*cacheSize = indexCount * sizeof(uint32_t) + vertexCount * 3 * sizeof(float) + sizeof(btBvhTriangleMeshShape);

	float* bvertices = new float[vertexCount*3];
	uint32_t* bindices = new uint32_t[indexCount];

	memcpy(bvertices, vertices, vertexCount*3*sizeof(float));
	memcpy(bindices, indices, indexCount*sizeof(uint32_t));

	btIndexedMesh mesh;
	mesh.m_numVertices = vertexCount;
	mesh.m_numTriangles = indexCount/3;
	mesh.m_triangleIndexStride = 3 * sizeof(uint32_t);
	mesh.m_vertexStride = 3 * sizeof(float);
	mesh.m_triangleIndexBase = (unsigned char*) bindices;
	mesh.m_vertexBase = (unsigned char*) bvertices;

	btTriangleIndexVertexArray* ivArray = new btTriangleIndexVertexArray;
	ivArray->addIndexedMesh(mesh);

	btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(ivArray, true, true);
	return shape;
}


btCollisionShape* CollisionModelBulletConverter::convert(const CollisionMesh* mesh, cachesize_t* cacheSize)
{
	return convert(mesh->getVertices(), mesh->getVertexCount(), mesh->getIndices(), mesh->getIndexCount(), cacheSize);
}


btCollisionShape* CollisionModelBulletConverter::convert(const CollisionModel* model, cachesize_t* cacheSize)
{
	btCompoundShape* shape = new btCompoundShape(true);

	if (cacheSize)
		*cacheSize = sizeof(btCompoundShape);

	bool convertedSomething = false;


	for (CollisionModel::ConstSphereIterator it = model->getSphereBegin() ; it != model->getSphereEnd() ; it++) {
		const CollisionSphere* sphere = *it;
		const Vector3& center = sphere->getCenter();
		cachesize_t sphereSize;
		btCollisionShape* sphereShape = convert(sphere, &sphereSize);
		shape->addChildShape(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f),
				btVector3(center.getX(), center.getY(), center.getZ())), sphereShape);

		convertedSomething = true;

		if (cacheSize)
			*cacheSize += sphereSize;
	}

	for (CollisionModel::ConstBoxIterator it = model->getBoxBegin() ; it != model->getBoxEnd() ; it++) {
		const CollisionBox* box = *it;
		const Vector3& min = box->getMinimum();
		const Vector3& max = box->getMaximum();
		Vector3 center = min + (max - min) * 0.5f;
		cachesize_t boxSize;
		btCollisionShape* boxShape = convert(box, &boxSize);
		shape->addChildShape(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f),
				btVector3(center.getX(), center.getY(), center.getZ())), boxShape);

		convertedSomething = true;

		if (cacheSize)
			*cacheSize += boxSize;
	}

	for (CollisionModel::ConstMeshIterator it = model->getMeshBegin() ; it != model->getMeshEnd() ; it++) {
		const CollisionMesh* mesh = *it;
		if (mesh->getIndexCount() > 0) {
			cachesize_t size;
			btCollisionShape* meshShape = convert(mesh, &size);
			shape->addChildShape(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f)), meshShape);

			convertedSomething = true;

			if (cacheSize)
				*cacheSize += size;
		}
	}

	if (!convertedSomething) {
		// I've been bitten by this before. Bullet crashes with an assertion failure when working with empty btCompoundShapes.
		delete shape;
		return new btEmptyShape;
	}

	return shape;
}
