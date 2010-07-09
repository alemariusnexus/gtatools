/*
 * IDETimedObject.h
 *
 *  Created on: 09.04.2010
 *      Author: alemariusnexus
 */

#ifndef IDETIMEDOBJECT_H_
#define IDETIMEDOBJECT_H_

#include "../gf_config.h"
#include "IDEStaticObject.h"

#define IDE_TYPE_TIMED_OBJECT 1


class IDETimedObject : public IDEStaticObject
{
public:
	IDETimedObject(int32_t id, const char* modelName, const char* textureName, int32_t numSubObjects,
			float* drawDistances, int32_t flags, int32_t timeOn, int32_t timeOff);
	virtual ~IDETimedObject();

	int32_t getTimeOn() const { return timeOn; }
	int32_t getTimeOff() const { return timeOff; }

	virtual idetype_t getType() const { return IDE_TYPE_TIMED_OBJECT; }

private:
	int32_t timeOn;
	int32_t timeOff;
};

#endif /* IDETIMEDOBJECT_H_ */
