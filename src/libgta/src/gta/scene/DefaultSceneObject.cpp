/*
 * DefaultSceneObject.cpp
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#include "DefaultSceneObject.h"
#include "../gl.h"
#include "../resource/ResourceCache.h"



DefaultSceneObject::DefaultSceneObject(MapItemDefinition* def, const Matrix4& modelMatrix,
			DefaultSceneObject* lodParent)
		: id(-1), def(def), modelMatrix(modelMatrix), lodParent(lodParent)
{
	const float* matData = modelMatrix.toArray();
	btMatrix3x3 matrix(matData[0], matData[4], matData[8], matData[1], matData[5], matData[9], matData[2],
			matData[6], matData[10]);
	motionState = new btDefaultMotionState(
			btTransform(matrix, btVector3(matData[12], matData[13], matData[14])));
	btRigidBody::btRigidBodyConstructionInfo info(0.0f, motionState, NULL);
	rigidBody = new btRigidBody(info);
	rigidBody->setRestitution(0.5f);
}


DefaultSceneObject::DefaultSceneObject(const DefaultSceneObject& other)
		: id(-1), def(other.def), modelMatrix(other.modelMatrix), lodParent(other.lodParent)
{
	const float* matData = modelMatrix.toArray();
	btMatrix3x3 matrix(matData[0], matData[4], matData[8], matData[1], matData[5], matData[9], matData[2],
			matData[6], matData[10]);
	motionState = new btDefaultMotionState(
			btTransform(matrix, btVector3(matData[12], matData[13], matData[14])));
	btRigidBody::btRigidBodyConstructionInfo info(0.0f, motionState, NULL);
	rigidBody = new btRigidBody(info);
	rigidBody->setRestitution(1.0f);
}


void DefaultSceneObject::setModelMatrix(const Matrix4& matrix)
{
	modelMatrix = matrix;
	const float* matData = modelMatrix.toArray();
	btMatrix3x3 btmatrix(matData[0], matData[4], matData[8], matData[1], matData[5], matData[9], matData[2],
			matData[6], matData[10]);
	btTransform trf(btmatrix, btVector3(matData[12], matData[13], matData[14]));
	rigidBody->getMotionState()->setWorldTransform(trf);
	rigidBody->setWorldTransform(trf);
}


void DefaultSceneObject::render(Matrix4& vpMatrix, GLuint mvpMatrixUniform)
{
	Matrix4 mat = vpMatrix * modelMatrix;
	const float* matData = mat.toArray();
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, matData);

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
}
