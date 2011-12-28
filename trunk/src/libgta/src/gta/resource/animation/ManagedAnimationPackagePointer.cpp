/*
 * ManagedAnimationPackagePointer.cpp
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
 */

#include "ManagedAnimationPackagePointer.h"
#include "AnimationCacheEntry.h"
#include "../../Engine.h"
#include "../ResourceCache.h"



AnimationPackage* ManagedAnimationPackagePointer::operator*()
{
	AnimationCacheEntry* entry = (AnimationCacheEntry*) pointer.getEntry();

	if (!entry)
		return NULL;

	return entry->getPackage();
}
