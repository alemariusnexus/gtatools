/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#include "COLBulletConverter.h"



btCollisionShape* COLBulletConverter::convert(const COLSphere& sphere, cachesize_t* cacheSize)
{
	if (cacheSize)
		*cacheSize = sizeof(btSphereShape);

	btSphereShape* shape = new btSphereShape(sphere.getRadius());
	return shape;
}


btCollisionShape* COLBulletConverter::convert(const COLBox& box, cachesize_t* cacheSize)
{
	if (cacheSize)
		*cacheSize = sizeof(btBoxShape);

	const Vector3& min = box.getMinimum();
	const Vector3& max = box.getMaximum();
	btBoxShape* shape = new btBoxShape(btVector3(
			(max.getX()-min.getX()) / 2.0f,
			(max.getY()-min.getY()) / 2.0f,
			(max.getZ()-min.getZ()) / 2.0f));
	return shape;
}


btCollisionShape* COLBulletConverter::convert(const float* vertices, int vertexCount, const COLFace* faces,
		int faceCount, cachesize_t* cacheSize)
{
	if (cacheSize)
		*cacheSize = faceCount * 3 * sizeof(uint32_t) + vertexCount * 3 * sizeof(float) + sizeof(btBvhTriangleMeshShape);

	float* bvertices = new float[vertexCount*3];
	uint32_t* bindices = new uint32_t[faceCount*3];

	memcpy(bvertices, vertices, vertexCount*3*sizeof(float));

	for (int i = 0 ; i < faceCount ; i++) {
		const COLFace& face = faces[i];
		const uint32_t* indices = face.getIndices();
		memcpy(bindices + i*3, indices, 3*sizeof(uint32_t));
	}

	btIndexedMesh mesh;
	mesh.m_numVertices = vertexCount;
	mesh.m_numTriangles = faceCount;
	mesh.m_triangleIndexStride = 3 * sizeof(uint32_t);
	mesh.m_vertexStride = 3 * sizeof(float);
	mesh.m_triangleIndexBase = (unsigned char*) bindices;
	mesh.m_vertexBase = (unsigned char*) bvertices;

	btTriangleIndexVertexArray* ivArray = new btTriangleIndexVertexArray;
	ivArray->addIndexedMesh(mesh);

	btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(ivArray, true, true);
	return shape;
}


btCollisionShape* COLBulletConverter::convert(const COLModel& model, cachesize_t* cacheSize)
{
	btCompoundShape* shape = new btCompoundShape(true);

	if (cacheSize)
		*cacheSize = sizeof(btCompoundShape);

	const COLSphere* spheres = model.getSpheres();
	uint32_t sphereCount = model.getSphereCount();

	for (uint32_t i = 0 ; i < sphereCount ; i++) {
		const COLSphere& sphere = spheres[i];
		const Vector3& center = sphere.getCenter();
		cachesize_t sphereSize;
		btCollisionShape* sphereShape = convert(sphere, &sphereSize);
		shape->addChildShape(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f),
				btVector3(center.getX(), center.getY(), center.getZ())), sphereShape);

		if (cacheSize)
			*cacheSize += sphereSize;
	}

	const COLBox* boxes = model.getBoxes();
	uint32_t boxCount = model.getBoxCount();

	for (uint32_t i = 0 ; i < boxCount ; i++) {
		const COLBox& box = boxes[i];
		const Vector3& min = box.getMinimum();
		const Vector3& max = box.getMaximum();
		Vector3 center = min + (max - min) * 0.5f;
		cachesize_t boxSize;
		btCollisionShape* boxShape = convert(box, &boxSize);
		shape->addChildShape(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f),
				btVector3(center.getX(), center.getY(), center.getZ())), boxShape);

		if (cacheSize)
			*cacheSize += boxSize;
	}

	if (model.getFaceCount() > 0) {
		cachesize_t size;
		btCollisionShape* meshShape = convert(model.getVertices(), model.getVertexCount(), model.getFaces(),
				model.getFaceCount(), &size);
		shape->addChildShape(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f)), meshShape);

		if (cacheSize)
			*cacheSize += size;
	}

	return shape;
}
