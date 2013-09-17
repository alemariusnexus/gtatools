/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef ANIMATEDMAPITEMDEFINITION_H_
#define ANIMATEDMAPITEMDEFINITION_H_

#include "MapItemDefinition.h"
#include "resource/animation/AnimationPackagePointer.h"
#include <gtaformats/ide/IDEAnimation.h>


class AnimatedMapItemDefinition : public MapItemDefinition {
public:
	AnimatedMapItemDefinition(MeshPointer* mptr, TextureSource* tsrc, CollisionShapePointer* cptr,
			ShadowMeshPointer* smptr, AnimationPackagePointer* aptr, float dd, unsigned int flags);
	AnimatedMapItemDefinition(const IDEAnimation& anim);
	AnimationPackagePointer* getAnimationPackagePointer() { return animPtr; }
	virtual int getType() const { return ItemTypeAnimatedMapItem; }
	CString getDefaultAnimation() const { return defaultAnim; }
	void setDefaultAnimation(const CString& anim) { defaultAnim = anim; }

private:
	AnimationPackagePointer* animPtr;
	CString defaultAnim;
};

#endif /* ANIMATEDMAPITEMDEFINITION_H_ */
