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

#include "StaticSceneObject.h"
#include "../gl.h"
#include "../resource/ResourceCache.h"
#include "SceneObjectDefinitionInfo.h"



// TODO When do we delete defInfo?


StaticSceneObject::StaticSceneObject(MapItemDefinition* def, const Matrix4& modelMatrix,
			StaticSceneObject* lodParent)
		: def(def), modelMatrix(modelMatrix), lodParent(lodParent), defInfo(NULL)
{
	setHasAlphaTransparency(def->hasAlphaTransparency());
	const float* matData = modelMatrix.toArray();
	btMatrix3x3 matrix(matData[0], matData[4], matData[8], matData[1], matData[5], matData[9], matData[2],
			matData[6], matData[10]);
	motionState = new btDefaultMotionState(
			btTransform(matrix, btVector3(matData[12], matData[13], matData[14])));
	btRigidBody::btRigidBodyConstructionInfo info(0.0f, motionState, NULL);
	rigidBody = new btRigidBody(info);
	rigidBody->setRestitution(0.5f);
}


StaticSceneObject::StaticSceneObject(const StaticSceneObject& other)
		: def(other.def), modelMatrix(other.modelMatrix), lodParent(other.lodParent),
		  defInfo(other.defInfo ? new SceneObjectDefinitionInfo(*other.defInfo) : NULL)
{
	setHasAlphaTransparency(other.hasAlphaTransparency());
	const float* matData = modelMatrix.toArray();
	btMatrix3x3 matrix(matData[0], matData[4], matData[8], matData[1], matData[5], matData[9], matData[2],
			matData[6], matData[10]);
	motionState = new btDefaultMotionState(
			btTransform(matrix, btVector3(matData[12], matData[13], matData[14])));
	btRigidBody::btRigidBodyConstructionInfo info(0.0f, motionState, NULL);
	rigidBody = new btRigidBody(info);
	rigidBody->setRestitution(1.0f);
}


void StaticSceneObject::setModelMatrix(const Matrix4& matrix)
{
	modelMatrix = matrix;
	const float* matData = modelMatrix.toArray();
	btMatrix3x3 btmatrix(matData[0], matData[4], matData[8], matData[1], matData[5], matData[9], matData[2],
			matData[6], matData[10]);
	btTransform trf(btmatrix, btVector3(matData[12], matData[13], matData[14]));
	rigidBody->getMotionState()->setWorldTransform(trf);
	rigidBody->setWorldTransform(trf);
}


/*void StaticSceneObject::render(Matrix4& vpMatrix, GLuint mvpMatrixUniform)
{


	if (Engine::getInstance()->isDrawingEnabled()) {
		def->render();
	}

	mat = vpMatrix;
	matData = mat.toArray();
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, matData);

	if (def->getCollisionShapePointer()) {
		btCollisionShape* shape = **def->getCollisionShapePointer();
		rigidBody->setCollisionShape(shape);
	}
}*/
