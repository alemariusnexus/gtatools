/*
 * AnimationCacheLoader.cpp
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
 */

#include "AnimationCacheLoader.h"
#include "AnimationCacheEntry.h"
#include "AnimationPackage.h"
#include <gtaformats/ifp/IFPLoader.h>



Engine::StringResourceCache::Entry* AnimationCacheLoader::load(CString key)
{
	File* file = index->find(key);

	if (!file)
		return NULL;

	AnimationPackage* pkg = new AnimationPackage;

	IFPLoader ifp(*file);

	IFPAnimation* ianim;
	while ((ianim = ifp.readAnimation())  !=  NULL) {
		Animation* anim = new Animation(ianim);
		pkg->addAnimation(CString(ianim->getName()).lower(), anim);
		delete ianim;
	}

	AnimationCacheEntry* entry = new AnimationCacheEntry(pkg);

	return entry;
}
