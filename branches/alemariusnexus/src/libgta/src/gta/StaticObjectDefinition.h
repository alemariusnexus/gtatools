/*
 * StaticObjectDefinition.h
 *
 *  Created on: 29.08.2010
 *      Author: alemariusnexus
 */

#ifndef STATICOBJECTDEFINITION_H_
#define STATICOBJECTDEFINITION_H_

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
	ResourceManager::hash_t txdName;
	Matrix4 modelMatrix;
	ResourceManager* res;
	bool txdKnown;
};

#endif /* STATICOBJECTDEFINITION_H_ */
