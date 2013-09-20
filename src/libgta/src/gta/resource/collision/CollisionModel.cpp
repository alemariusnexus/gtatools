/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#include "CollisionModel.h"




CollisionModel::CollisionModel(const COLModel& model)
{
	if (model.getFaceCount() != 0)
		addMesh(new CollisionMesh(model));

	const COLBox* colBoxes = model.getBoxes();
	uint32_t numBoxes = model.getBoxCount();

	const COLSphere* colSpheres = model.getSpheres();
	uint32_t numSpheres = model.getSphereCount();

	for (uint32_t i = 0 ; i < numBoxes ; i++)
		addBox(new CollisionBox(colBoxes[i]));

	for (uint32_t i = 0 ; i < numSpheres ; i++)
		addSphere(new CollisionSphere(colSpheres[i]));

	const COLBounds& bounds = model.getBounds();

	boundingSphereCenter = bounds.getCenter();
	boundingSphereRadius = bounds.getRadius();

	boundingBoxMin = bounds.getMinimum();
	boundingBoxMax = bounds.getMaximum();
}


void CollisionModel::setBoundingSphere(const Vector3& center, float radius)
{
	boundingSphereCenter = center;
	boundingSphereRadius = radius;
}


void CollisionModel::setBoundingBox(const Vector3& min, const Vector3& max)
{
	boundingBoxMin = min;
	boundingBoxMax = max;
}


cachesize_t CollisionModel::getCacheSize() const
{
	cachesize_t size = sizeof(CollisionModel);

	for (ConstMeshIterator it = meshes.begin() ; it != meshes.end() ; it++) {
		size += (*it)->getCacheSize();
	}

	size += boxes.size() * sizeof(CollisionBox);
	size += spheres.size() * sizeof(CollisionSphere);

	return size;
}
