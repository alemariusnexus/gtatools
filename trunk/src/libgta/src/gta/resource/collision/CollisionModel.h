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

#ifndef COLLISIONMODEL_H_
#define COLLISIONMODEL_H_

#include <gta/config.h>
#include <gtaformats/col/COLModel.h>
#include "CollisionMesh.h"
#include "CollisionBox.h"
#include "CollisionSphere.h"
#include "../ResourceCache.h"
#include <list>

using std::list;



class CollisionModel
{
public:
	typedef list<CollisionMesh*> MeshList;
	typedef MeshList::iterator MeshIterator;
	typedef MeshList::const_iterator ConstMeshIterator;

	typedef list<CollisionBox*> BoxList;
	typedef BoxList::iterator BoxIterator;
	typedef BoxList::const_iterator ConstBoxIterator;

	typedef list<CollisionSphere*> SphereList;
	typedef SphereList::iterator SphereIterator;
	typedef SphereList::const_iterator ConstSphereIterator;

public:
	CollisionModel() {}
	CollisionModel(const COLModel& model);

	void setBoundingSphere(const Vector3& center, float radius);
	void setBoundingBox(const Vector3& min, const Vector3& max);

	void getBoundingSphere(Vector3& center, float& radius) const { center = boundingSphereCenter; radius = boundingSphereRadius; }
	void getBoundingBox(Vector3& min, Vector3& max) const { min = boundingBoxMin; max = boundingBoxMax; }

	void addMesh(CollisionMesh* mesh) { meshes.push_back(mesh); }
	void addBox(CollisionBox* box) { boxes.push_back(box); }
	void addSphere(CollisionSphere* sphere) { spheres.push_back(sphere); }

	size_t getMeshCount() const { return meshes.size(); }
	size_t getBoxCount() const { return boxes.size(); }
	size_t getSphereCount() const { return spheres.size(); }

	MeshIterator getMeshBegin() { return meshes.begin(); }
	ConstMeshIterator getMeshBegin() const { return meshes.begin(); }
	MeshIterator getMeshEnd() { return meshes.end(); }
	ConstMeshIterator getMeshEnd() const { return meshes.end(); }

	BoxIterator getBoxBegin() { return boxes.begin(); }
	ConstBoxIterator getBoxBegin() const { return boxes.begin(); }
	BoxIterator getBoxEnd() { return boxes.end(); }
	ConstBoxIterator getBoxEnd() const { return boxes.end(); }

	SphereIterator getSphereBegin() { return spheres.begin(); }
	ConstSphereIterator getSphereBegin() const { return spheres.begin(); }
	SphereIterator getSphereEnd() { return spheres.end(); }
	ConstSphereIterator getSphereEnd() const { return spheres.end(); }

	cachesize_t getCacheSize() const;

private:
	MeshList meshes;
	BoxList boxes;
	SphereList spheres;

	Vector3 boundingSphereCenter;
	float boundingSphereRadius;

	Vector3 boundingBoxMin;
	Vector3 boundingBoxMax;
};

#endif /* COLLISIONMODEL_H_ */
