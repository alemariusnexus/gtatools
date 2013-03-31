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

#ifndef COLBULLETCONVERTER_H_
#define COLBULLETCONVERTER_H_

#include <btBulletDynamicsCommon.h>
#include <gtaformats/gtacol.h>
#include "../ResourceCache.h"


class COLBulletConverter {
public:
	btCollisionShape* convert(const COLSphere& sphere, cachesize_t* cacheSize = NULL);
	btCollisionShape* convert(const COLBox& box, cachesize_t* cacheSize = NULL);
	btCollisionShape* convert(const float* vertices, int vertexCount, const COLFace* faces, int faceCount,
			cachesize_t* cacheSize = NULL);
	btCollisionShape* convert(const COLModel& model, cachesize_t* cacheSize = NULL);
};

#endif /* COLBULLETCONVERTER_H_ */
