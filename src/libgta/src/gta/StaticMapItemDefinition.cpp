/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "StaticMapItemDefinition.h"
#include "resource/mesh/ManagedMeshPointer.h"
#include "resource/texture/ManagedTextureSource.h"
#include "resource/collision/ManagedCollisionShapePointer.h"
#include "resource/smesh/ManagedShadowMeshPointer.h"
#include "resource/physics/ManagedPhysicsPointer.h"
#include "gl.h"
#include "GLException.h"



StaticMapItemDefinition::StaticMapItemDefinition(MeshPointer* meshPtr, TextureSource* texSrc,
		CollisionShapePointer* colPtr, ShadowMeshPointer* smeshPtr, PhysicsPointer* pptr, float drawDist, unsigned int flags)
		: MapItemDefinition(meshPtr, texSrc, colPtr, smeshPtr, pptr, drawDist, flags), initedProgram(NULL)
{
}


StaticMapItemDefinition::StaticMapItemDefinition(IDEStaticObject& object)
		: initedProgram(NULL)
{
	drawDist = object.getDrawDistances()[object.getNumSubObjects() - 1];
	CString lMeshName(object.getModelName());
	lMeshName.lower();
	meshPtr = new ManagedMeshPointer(lMeshName);
	texSrc = new ManagedTextureSource(CString(object.getTXDArchiveName()).lower());
	colPtr = new ManagedCollisionShapePointer(lMeshName);
	smeshPtr = new ManagedShadowMeshPointer(lMeshName);
	physicsPtr = new ManagedPhysicsPointer(lMeshName);

	if (	(object.getFlags() & (IDEStaticObject::AlphaTransparency1 | IDEStaticObject::AlphaTransparency2))
			!= 0
	) {
		flags |= AlphaTransparency;
	}
}


StaticMapItemDefinition::~StaticMapItemDefinition()
{
	delete meshPtr;
	delete texSrc;
	delete colPtr;
	delete smeshPtr;
	delete physicsPtr;
}
