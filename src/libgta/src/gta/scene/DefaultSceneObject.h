/*
 * DefaultSceneObject.h
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#ifndef DEFAULTSCENEOBJECT_H_
#define DEFAULTSCENEOBJECT_H_

#include <gtaformats/util/math/Matrix4.h>
#include <gtaformats/gtaipl.h>
#include "../ItemDefinition.h"


class DefaultSceneObject {
public:
	DefaultSceneObject(ItemDefinition* def, const Matrix4& modelMatrix = Matrix4(),
			DefaultSceneObject* lodParent = NULL);
	ItemDefinition* getDefinition() { return def; }
	Matrix4& getModelMatrix() { return modelMatrix; }
	DefaultSceneObject* getLODParent() { return lodParent; }
	void setLODParent(DefaultSceneObject* parent) { lodParent = parent; }

private:
	ItemDefinition* def;
	Matrix4 modelMatrix;
	DefaultSceneObject* lodParent;
};

#endif /* DEFAULTSCENEOBJECT_H_ */
