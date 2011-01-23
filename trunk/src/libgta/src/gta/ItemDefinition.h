/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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
 */

#ifndef ITEMDEFINITION_H_
#define ITEMDEFINITION_H_

#include "config.h"
#include "Engine.h"
#include "Mesh.h"
#include <gtaformats/ide/IDEStaticObject.h>
#include "MeshPointer.h"
#include "TextureSource.h"



class ItemDefinition {
public:
	ItemDefinition(MeshPointer* meshPtr, TextureSource* texSrc, float drawDistance);
	ItemDefinition(const IDEStaticObject& object);
	~ItemDefinition();
	float getDrawDistance() const { return drawDistance; }
	virtual void render();

private:
	MeshPointer* meshPtr;
	TextureSource* texSrc;
	//hash_t txdHash;
	float drawDistance;
};

#endif /* ITEMDEFINITION_H_ */