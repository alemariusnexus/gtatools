/*
 * DefaultSceneObject.cpp
 *
 *  Created on: 07.03.2011
 *      Author: alemariusnexus
 */

#include "DefaultSceneObject.h"



DefaultSceneObject::DefaultSceneObject(ItemDefinition* def, const Matrix4& modelMatrix,
			DefaultSceneObject* lodParent)
		: id(-1), def(def), modelMatrix(modelMatrix), lodParent(lodParent)
{
}