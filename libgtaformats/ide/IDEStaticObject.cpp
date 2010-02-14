/*
 * IDEStaticObject.cpp
 *
 *  Created on: 09.02.2010
 *      Author: alemariusnexus
 */

#include "IDEStaticObject.h"


IDEStaticObject::IDEStaticObject(int32_t id, char* modelName, char* textureName, int32_t numSubObjects,
			float* drawDistances, int32_t flags)
		: id(id), modelName(modelName), textureName(textureName), numSubObjects(numSubObjects),
		  drawDistances(drawDistances), flags(flags)
{
}

