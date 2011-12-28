/*
 * AnimatedMapItemDefinition.h
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
 */

#ifndef ANIMATEDMAPITEMDEFINITION_H_
#define ANIMATEDMAPITEMDEFINITION_H_

#include "MapItemDefinition.h"
#include "resource/animation/AnimationPackagePointer.h"


class AnimatedMapItemDefinition : public MapItemDefinition {
public:
	AnimatedMapItemDefinition(MeshPointer* mptr, TextureSource* tsrc, CollisionShapePointer* cptr,
			AnimationPackagePointer* aptr, float dd, unsigned int flags);
	AnimationPackagePointer* getAnimationPackagePointer() { return animPtr; }

private:
	AnimationPackagePointer* animPtr;
};

#endif /* ANIMATEDMAPITEMDEFINITION_H_ */
