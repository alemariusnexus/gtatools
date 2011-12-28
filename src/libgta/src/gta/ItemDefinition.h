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
#include "resource/mesh/Mesh.h"
#include <gtaformats/ide/IDEStaticObject.h>
#include "resource/mesh/MeshPointer.h"
#include "resource/texture/TextureSource.h"
#include "resource/collision/CollisionShapePointer.h"



class ItemDefinition {
public:
	virtual ~ItemDefinition() {}
	virtual bool isVisible() const { return true; }
};

#endif /* ITEMDEFINITION_H_ */
