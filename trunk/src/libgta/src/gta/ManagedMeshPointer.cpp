/*
 * ManagedMeshPointer.cpp
 *
 *  Created on: 06.11.2010
 *      Author: alemariusnexus
 */

#include "ManagedMeshPointer.h"
#include "ResourceManager.h"



Mesh* ManagedMeshPointer::operator*()
{
	ResourceManager* resMgr = Engine::getInstance()->getResourceManager();
	Mesh* mesh;
	if (resMgr->getMesh(meshHash, mesh)) {
		return mesh;
	} else {
		return NULL;
	}
}
