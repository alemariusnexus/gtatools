/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

#include "AnimatedMapItemDefinition.h"
#include "resource/mesh/ManagedMeshPointer.h"
#include "resource/texture/ManagedTextureSource.h"
#include "resource/collision/ManagedCollisionShapePointer.h"
#include "resource/animation/ManagedAnimationPackagePointer.h"




AnimatedMapItemDefinition::AnimatedMapItemDefinition(MeshPointer* mptr, TextureSource* tsrc,
		CollisionShapePointer* cptr, AnimationPackagePointer* aptr, float dd, unsigned int flags)
		: MapItemDefinition(mptr, tsrc, cptr, dd, flags), animPtr(aptr)
{
}


AnimatedMapItemDefinition::AnimatedMapItemDefinition(const IDEAnimation& anim)
{
	drawDist = anim.getDrawDist();
	CString lMeshName(anim.getModelName());
	lMeshName.lower();
	meshPtr = new ManagedMeshPointer(lMeshName);
	texSrc = new ManagedTextureSource(CString(anim.getTextureName()).lower());
	colPtr = new ManagedCollisionShapePointer(lMeshName);
	animPtr = new ManagedAnimationPackagePointer(CString(anim.getAnimationName()).lower());
	defaultAnim = lMeshName;

	if (	(anim.getFlags() & (IDEStaticObject::AlphaTransparency1 | IDEStaticObject::AlphaTransparency2))
			!= 0
	) {
		flags |= AlphaTransparency;
	}
}
