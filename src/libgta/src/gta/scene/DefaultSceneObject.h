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
#include <btBulletDynamicsCommon.h>


class DefaultSceneObject {
public:
	DefaultSceneObject(ItemDefinition* def, const Matrix4& modelMatrix = Matrix4(),
			DefaultSceneObject* lodParent = NULL);
	DefaultSceneObject(const DefaultSceneObject& other);
	ItemDefinition* getDefinition() { return def; }
	const ItemDefinition* getDefinition() const { return def; }
	Matrix4& getModelMatrix() { return modelMatrix; }
	const Matrix4& getModelMatrix() const { return modelMatrix; }
	void setModelMatrix(const Matrix4& matrix);
	DefaultSceneObject* getLODParent() { return lodParent; }
	const DefaultSceneObject* getLODParent() const { return lodParent; }
	void setLODParent(DefaultSceneObject* parent) { lodParent = parent; }
	int getLODHierarchyDepth() const { return lodParent ? lodParent->getLODHierarchyDepth()+1 : 0; }
	int getID() const { return id; }
	void setID(int id) { this->id = id; }
	btRigidBody* getRigidBody() { return rigidBody; }
	void render(Matrix4& vpMatrix, GLuint mvpMatrixUniform);

private:
	int id;
	ItemDefinition* def;
	Matrix4 modelMatrix;
	DefaultSceneObject* lodParent;
	btRigidBody* rigidBody;
	btMotionState* motionState;
};

#endif /* DEFAULTSCENEOBJECT_H_ */
