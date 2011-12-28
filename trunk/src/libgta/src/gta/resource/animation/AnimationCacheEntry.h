/*
 * AnimationCacheEntry.h
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
 */

#ifndef ANIMATIONCACHEENTRY_H_
#define ANIMATIONCACHEENTRY_H_

#include "../ResourceCache.h"
#include "AnimationPackage.h"


class AnimationCacheEntry : public Engine::StringResourceCache::Entry {
public:
	AnimationCacheEntry(AnimationPackage* pkg) : pkg(pkg) {}
	virtual ~AnimationCacheEntry();
	virtual cachesize_t getSize() const { return pkg->getSize(); }
	AnimationPackage* getPackage() const { return pkg; }

private:
	AnimationPackage* pkg;
};

#endif /* ANIMATIONCACHEENTRY_H_ */
