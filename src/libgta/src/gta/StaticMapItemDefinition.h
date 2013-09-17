/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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
#ifndef STATICMAPITEMDEFINITION_H_
#define STATICMAPITEMDEFINITION_H_

#include "MapItemDefinition.h"
#include "ShaderProgram.h"
#include <gtaformats/ide/IDEStaticObject.h>


class StaticMapItemDefinition : public MapItemDefinition {
public:
	StaticMapItemDefinition(MeshPointer* meshPtr, TextureSource* texSrc, CollisionShapePointer* colPtr,
			ShadowMeshPointer* smeshPtr, float drawDist, unsigned int flags = 0);
	StaticMapItemDefinition(IDEStaticObject& object);
	~StaticMapItemDefinition();
	virtual int getType() const { return ItemTypeStaticMapItem; }

private:
	void initShaderLocations();

private:
	ShaderProgram* initedProgram;
	GLint texturedUniform, materialColorUniform, vertexColorsUniform;
	GLint textureUniform;
	GLint vertexAttrib, normalAttrib, colorAttrib, texCoordAttrib;
};

#endif /* STATICMAPITEMDEFINITION_H_ */
