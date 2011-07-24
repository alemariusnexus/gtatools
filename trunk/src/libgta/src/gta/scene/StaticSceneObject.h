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

#ifndef STATICSCENEOBJECT_H_
#define STATICSCENEOBJECT_H_

#include "SceneObject.h"
#include <gtaformats/util/math/Matrix4.h>
#include <gtaformats/gtaipl.h>
#include "../MapItemDefinition.h"
#include <btBulletDynamicsCommon.h>


class StaticSceneObject : public SceneObject {
public:
	StaticSceneObject(MapItemDefinition* def, const Matrix4& modelMatrix = Matrix4(),
			StaticSceneObject* lodParent = NULL);
	StaticSceneObject(const StaticSceneObject& other);
	MapItemDefinition* getDefinition() { return def; }
	const MapItemDefinition* getDefinition() const { return def; }
	Matrix4& getModelMatrix() { return modelMatrix; }
	const Matrix4& getModelMatrix() const { return modelMatrix; }
	void setModelMatrix(const Matrix4& matrix);
	StaticSceneObject* getLODParent() { return lodParent; }
	const StaticSceneObject* getLODParent() const { return lodParent; }
	void setLODParent(StaticSceneObject* parent) { lodParent = parent; }
	int getLODHierarchyDepth() const { return lodParent ? lodParent->getLODHierarchyDepth()+1 : 0; }
	int getID() const { return id; }
	void setID(int id) { this->id = id; }
	btRigidBody* getRigidBody() { return rigidBody; }
	virtual bool isVisible() const { return def->isVisible(); }
	virtual int getType() const { return SceneObjectStatic; }

private:
	int id;
	int flags;
	MapItemDefinition* def;
	Matrix4 modelMatrix;
	StaticSceneObject* lodParent;
	btRigidBody* rigidBody;
	btMotionState* motionState;
};

#endif /* STATICSCENEOBJECT_H_ */
