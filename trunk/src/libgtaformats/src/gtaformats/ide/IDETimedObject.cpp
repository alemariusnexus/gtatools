/*
 * IDETimedObject.cpp
 *
 *  Created on: 09.04.2010
 *      Author: alemariusnexus
 */

#include "IDETimedObject.h"
#include <cstring>


IDETimedObject::IDETimedObject(int32_t id, const char* modelName, const char* texName,
			int32_t numSubObjects, float* drawDistances, int32_t flags, int32_t timeOn, int32_t timeOff)
		: IDEStaticObject(id, modelName, texName, numSubObjects, drawDistances, flags),
		  timeOn(timeOn), timeOff(timeOff)
{
}


IDETimedObject::~IDETimedObject()
{

}

