/*
 * AnimationPackagePointer.h
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
 */

#ifndef ANIMATIONPACKAGEPOINTER_H_
#define ANIMATIONPACKAGEPOINTER_H_

#include "AnimationPackage.h"


class AnimationPackagePointer {
public:
	virtual AnimationPackagePointer* clone() const = 0;
	virtual AnimationPackage* operator*() = 0;
};

#endif /* ANIMATIONPACKAGEPOINTER_H_ */
