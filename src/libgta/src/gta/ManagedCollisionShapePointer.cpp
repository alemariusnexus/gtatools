/*
 * ManagedCollisionShapePointer.cpp
 *
 *  Created on: 13.02.2011
 *      Author: alemariusnexus
 */

#include "ManagedCollisionShapePointer.h"
//#include "ResourceManager.h"


CollisionShapePointer* ManagedCollisionShapePointer::clone() const
{
	return new ManagedCollisionShapePointer(*this);
}


btCollisionShape* ManagedCollisionShapePointer::operator*()
{
	/*ResourceManager* resMgr = Engine::getInstance()->getResourceManager();
	return resMgr->getCollisionShape(name);*/
	return NULL;
}
