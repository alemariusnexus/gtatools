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
	const ItemDefinition* getDefinition() const { return def; }
	Matrix4& getModelMatrix() { return modelMatrix; }
	const Matrix4& getModelMatrix() const { return modelMatrix; }
	DefaultSceneObject* getLODParent() { return lodParent; }
	const DefaultSceneObject* getLODParent() const { return lodParent; }
	void setLODParent(DefaultSceneObject* parent) { lodParent = parent; }
	int getLODHierarchyDepth() const { return lodParent ? lodParent->getLODHierarchyDepth()+1 : 0; }
	int getID() const { return id; }
	void setID(int id) { this->id = id; }

private:
	int id;
	ItemDefinition* def;
	Matrix4 modelMatrix;
	DefaultSceneObject* lodParent;
};

#endif /* DEFAULTSCENEOBJECT_H_ */
