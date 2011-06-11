/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#ifndef ITEMDEFINITION_H_
#define ITEMDEFINITION_H_

#include <gta/config.h>
#include "Engine.h"
#include "Mesh.h"
#include <gtaformats/ide/IDEStaticObject.h>
#include "MeshPointer.h"
#include "TextureSource.h"
#include "CollisionShapePointer.h"



class ItemDefinition {
public:
	~ItemDefinition();
	TextureSource* getTextureSource() { return texSrc; }
	const TextureSource* getTextureSource() const { return texSrc; }
	void setTextureSource(TextureSource* source) { if (texSrc) delete texSrc; texSrc = source; }
	MeshPointer* getMeshPointer() { return meshPtr; }
	const MeshPointer* getMeshPointer() const { return meshPtr; }
	void setMeshPointer(MeshPointer* ptr) { if (meshPtr) delete meshPtr; meshPtr = ptr; }
	CollisionShapePointer* getCollisionShapePointer() { return colShapePtr; }
	const CollisionShapePointer* getCollisionShapePointer() const { return colShapePtr; }
	void setCollisionShapePointer(CollisionShapePointer* ptr)
			{ if (colShapePtr) delete colShapePtr; colShapePtr = ptr; }
	float getDrawDistance() const { return drawDistance; }
	float getDrawDistanceSquarred() const { return drawDistanceSquarred; }
	void setDrawDistance(float dd) { drawDistance = dd; drawDistanceSquarred = dd*dd; }
	virtual bool isVisible() const;
	virtual void render();

protected:
	ItemDefinition(MeshPointer* meshPtr, TextureSource* texSrc, float drawDistance);
	ItemDefinition();

private:
	void initShaderLocations();

protected:
	MeshPointer* meshPtr;
	TextureSource* texSrc;
	CollisionShapePointer* colShapePtr;
	float drawDistance;
	float drawDistanceSquarred;
	GLint vertexAttrib, normalAttrib, texCoordAttrib, colorAttrib;
	GLint textureUniform, texturedUniform, materialColorUniform, vertexColorsUniform;
};

#endif /* ITEMDEFINITION_H_ */
