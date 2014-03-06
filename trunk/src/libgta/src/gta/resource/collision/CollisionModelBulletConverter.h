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

#ifndef COLLISIONMODELBULLETCONVERTER_H_
#define COLLISIONMODELBULLETCONVERTER_H_

#include <btBulletDynamicsCommon.h>
#include <gtaformats/gtacol.h>
#include "../ResourceCache.h"
#include "CollisionBox.h"
#include "CollisionSphere.h"
#include "CollisionMesh.h"
#include "CollisionModel.h"


class CollisionModelBulletConverter {
public:
	btCollisionShape* convert(const CollisionSphere* sphere, cachesize_t* cacheSize = NULL);
	btCollisionShape* convert(const CollisionBox* box, cachesize_t* cacheSize = NULL);
	btCollisionShape* convert(const float* vertices, int vertexCount, const uint32_t* indices, int faceCount,
			cachesize_t* cacheSize = NULL);
	btCollisionShape* convert(const CollisionMesh* mesh, cachesize_t* cacheSize = NULL);
	btCollisionShape* convert(const CollisionModel* model, cachesize_t* cacheSize = NULL);
};

#endif /* COLLISIONMODELBULLETCONVERTER_H_ */
