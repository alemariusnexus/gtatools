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

#ifndef STATICOBJECTDEFINITION_H_
#define STATICOBJECTDEFINITION_H_

#include "config.h"
#include "ObjectDefinition.h"
#include <GL/glew.h>
#include <gtaformats/gtadff.h>
#include <gtaformats/util/math/Matrix4.h>
#include "ResourceManager.h"



class StaticObjectDefinition : public ObjectDefinition {
public:
	StaticObjectDefinition(DFFMesh* mesh, const char* txdName, ResourceManager* res);
	StaticObjectDefinition(DFFGeometry* geom, const char* txdName, ResourceManager* res);
	StaticObjectDefinition(DFFGeometry* geom, DFFGeometryPart* part, const char* txdName,
			ResourceManager* res);
	~StaticObjectDefinition();
	virtual void draw();

private:
	void init(DFFGeometry* geom, DFFGeometryPart* part = NULL);
	void initPart(DFFGeometryPart* part, int index);

private:
	int32_t vertexCount;
	int32_t* indexCounts;
	int32_t partCount;
	GLuint* partTextures;
	bool* partHasTextures;
	int16_t flags;
	GLuint dataBuffer;
	GLuint indexBuffer;
	hash_t txdName;
	Matrix4 modelMatrix;
	ResourceManager* res;
	bool txdKnown;
};

#endif /* STATICOBJECTDEFINITION_H_ */
