/*
 * ManagedAnimationPackagePointer.h
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
 */

#ifndef MANAGEDANIMATIONPACKAGEPOINTER_H_
#define MANAGEDANIMATIONPACKAGEPOINTER_H_

#include "AnimationPackagePointer.h"
#include <gtaformats/util/CString.h>
#include "../ResourceCache.h"
#include "../../Engine.h"



class ManagedAnimationPackagePointer : public AnimationPackagePointer {
public:
	ManagedAnimationPackagePointer(CString name)
			: pointer(Engine::getInstance()->getAnimationCache()->getEntryPointer(name)) {}
	ManagedAnimationPackagePointer(const ManagedAnimationPackagePointer& other) : pointer(other.pointer) {}
	virtual AnimationPackagePointer* clone() const { return new ManagedAnimationPackagePointer(*this); }
	virtual AnimationPackage* operator*();

private:
	Engine::StringResourceCache::Pointer pointer;
};

#endif /* MANAGEDANIMATIONPACKAGEPOINTER_H_ */
