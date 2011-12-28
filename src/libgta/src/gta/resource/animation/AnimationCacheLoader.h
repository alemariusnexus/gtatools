/*
 * AnimationCacheLoader.h
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
 */

#ifndef ANIMATIONCACHELOADER_H_
#define ANIMATIONCACHELOADER_H_

#include "../ResourceCache.h"
#include "AnimationIndexer.h"



class AnimationCacheLoader : public Engine::StringResourceCache::EntryLoader {
public:
	AnimationCacheLoader(AnimationIndexer* index) : index(index) {}
	virtual Engine::StringResourceCache::Entry* load(CString key);

private:
	AnimationIndexer* index;
};

#endif /* ANIMATIONCACHELOADER_H_ */
