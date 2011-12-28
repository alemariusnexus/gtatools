/*
 * AnimationPackage.h
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
 */

#ifndef ANIMATIONPACKAGE_H_
#define ANIMATIONPACKAGE_H_

#include "../../Animation.h"
#include "../ResourceCache.h"
#include <gtaformats/util/strutil.h>
#include <gtaformats/util/CString.h>
#include <map>

using std::map;



class AnimationPackage {
private:
	typedef map<CString, Animation*> AnimMap;

public:
	AnimationPackage() : size(sizeof(AnimMap) + sizeof(cachesize_t)) {}
	~AnimationPackage();
	cachesize_t getSize() const { return size; }
	void addAnimation(const CString& name, Animation* anim);
	Animation* find(const CString& name);
	Animation* operator[](const CString& name) { return find(name); }

private:
	AnimMap anims;
	cachesize_t size;
};

#endif /* ANIMATIONPACKAGE_H_ */
