/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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
#include "ManagedMeshPointer.h"
#include "ManagedTextureSource.h"
#include "ManagedCollisionShapePointer.h"



StaticMapItemDefinition::StaticMapItemDefinition(MeshPointer* meshPtr, TextureSource* texSrc, float drawDist)
		: MapItemDefinition(meshPtr, texSrc, drawDist)
{
}


StaticMapItemDefinition::StaticMapItemDefinition(IDEStaticObject& object)
{
	setDrawDistance(object.getDrawDistances()[0]);
	char* lMeshName = new char[strlen(object.getModelName())+1];
	char* lTexName = new char[strlen(object.getTextureName())+1];
	strtolower(lMeshName, object.getModelName());
	strtolower(lTexName, object.getTextureName());
	meshPtr = new ManagedMeshPointer(Hash(lMeshName));
	texSrc = new ManagedTextureSource(Hash(lTexName));
	colShapePtr = new ManagedCollisionShapePointer(lMeshName);
	delete[] lMeshName;
	delete[] lTexName;
}
