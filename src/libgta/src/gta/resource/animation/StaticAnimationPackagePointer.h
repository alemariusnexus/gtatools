/*
 * StaticAnimationPackagePointer.h
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
 */

#ifndef STATICANIMATIONPACKAGEPOINTER_H_
#define STATICANIMATIONPACKAGEPOINTER_H_

#include "AnimationPackagePointer.h"


class StaticAnimationPackagePointer : public AnimationPackagePointer {
public:
	StaticAnimationPackagePointer(AnimationPackage* pkg) : pkg(pkg) {}
	StaticAnimationPackagePointer(const StaticAnimationPackagePointer& other) : pkg(other.pkg) {}
	virtual AnimationPackagePointer* clone() const { return new StaticAnimationPackagePointer(*this); }
	virtual AnimationPackage* operator*() { return pkg; }

private:
	AnimationPackage* pkg;
};

#endif /* STATICANIMATIONPACKAGEPOINTER_H_ */
