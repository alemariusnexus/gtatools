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

#include "PhysicsCacheLoader.h"
#include "../../Engine.h"
#include "PhysicsCacheEntry.h"
#include "../collision/CollisionMeshCacheEntry.h"
#include "../collision/CollisionModelBulletConverter.h"





PhysicsCacheLoader::PhysicsCacheLoader()
{
}


Engine::StringResourceCache::Entry* PhysicsCacheLoader::load(CString name)
{
	Engine* engine = Engine::getInstance();

	CollisionModelBulletConverter conv;

	Engine::StringResourceCache* colCache = engine->getCollisionMeshCache();
	Engine::StringResourceCache::Pointer ptr = colCache->getEntryPointer(name);
	CollisionMeshCacheEntry* colEntry = (CollisionMeshCacheEntry*) ptr.getEntry(true);

	if (!colEntry)
		return NULL;

	CollisionModel* model = **colEntry;

	cachesize_t size;
	btCollisionShape* shape = conv.convert(model, &size);
	shape->setMargin(2.0f);

	ptr.release();

	PhysicsCacheEntry* cacheEntry = new PhysicsCacheEntry(shape, size);
	return cacheEntry;
}
