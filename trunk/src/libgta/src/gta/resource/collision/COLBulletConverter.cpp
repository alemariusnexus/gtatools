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



btCollisionShape* COLBulletConverter::convert(const COLSphere& sphere)
{
	btSphereShape* shape = new btSphereShape(sphere.getRadius());
	return shape;
}


btCollisionShape* COLBulletConverter::convert(const COLBox& box)
{
	const Vector3& min = box.getMinimum();
	const Vector3& max = box.getMaximum();
	btBoxShape* shape = new btBoxShape(btVector3(
			(max.getX()-min.getX()) / 2.0f,
			(max.getY()-min.getY()) / 2.0f,
			(max.getZ()-min.getZ()) / 2.0f));
	return shape;
}


btCollisionShape* COLBulletConverter::convert(const float* vertices, int vertexCount, const COLFace* faces,
		int faceCount)
{
	btTriangleMesh* mesh = new btTriangleMesh(true, false);
	mesh->preallocateVertices(faceCount*3);

	for (int i = 0 ; i < faceCount ; i++) {
		const COLFace& face = faces[i];
		const uint32_t* indices = face.getIndices();
		const float* v1 = vertices + indices[0]*3;
		const float* v2 = vertices + indices[1]*3;
		const float* v3 = vertices + indices[2]*3;
		mesh->addTriangle(btVector3(v1[0], v1[1], v1[2]), btVector3(v2[0], v2[1], v2[2]),
				btVector3(v3[0], v3[1], v3[2]));
	}

	btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(mesh, true, true);
	return shape;
}


btCollisionShape* COLBulletConverter::convert(const COLModel& model)
{
	btCompoundShape* shape = new btCompoundShape(true);

	const COLSphere* spheres = model.getSpheres();
	uint32_t sphereCount = model.getSphereCount();

	for (uint32_t i = 0 ; i < sphereCount ; i++) {
		const COLSphere& sphere = spheres[i];
		const Vector3& center = sphere.getCenter();
		btCollisionShape* sphereShape = convert(sphere);
		shape->addChildShape(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f),
				btVector3(center.getX(), center.getY(), center.getZ())), sphereShape);
	}

	const COLBox* boxes = model.getBoxes();
	uint32_t boxCount = model.getBoxCount();

	for (uint32_t i = 0 ; i < boxCount ; i++) {
		const COLBox& box = boxes[i];
		const Vector3& min = box.getMinimum();
		const Vector3& max = box.getMaximum();
		Vector3 center = min + (max - min) * 0.5f;
		btCollisionShape* boxShape = convert(box);
		shape->addChildShape(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f),
				btVector3(center.getX(), center.getY(), center.getZ())), boxShape);
	}

	if (model.getFaceCount() > 0) {
		btCollisionShape* meshShape = convert(model.getVertices(), model.getVertexCount(), model.getFaces(),
				model.getFaceCount());
		shape->addChildShape(btTransform(btQuaternion(0.0f, 0.0f, 0.0f, 1.0f)), meshShape);
	}

	return shape;
}
