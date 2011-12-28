/*
 * AnimatedMapItemDefinition.cpp
 *
 *  Created on: 04.09.2011
 *      Author: alemariusnexus
 */

#include "AnimatedMapItemDefinition.h"




AnimatedMapItemDefinition::AnimatedMapItemDefinition(MeshPointer* mptr, TextureSource* tsrc,
		CollisionShapePointer* cptr, AnimationPackagePointer* aptr, float dd, unsigned int flags)
		: MapItemDefinition(mptr, tsrc, cptr, dd, flags), animPtr(aptr)
{
}
