/*
 * VisualSceneObject.cpp
 *
 *  Created on: 01.01.2012
 *      Author: alemariusnexus
 */

#include "VisualSceneObject.h"



void VisualSceneObject::setModelMatrix(const Matrix4& matrix)
{
	modelMatrix = matrix;
	const float* matData = modelMatrix.toArray();
}
