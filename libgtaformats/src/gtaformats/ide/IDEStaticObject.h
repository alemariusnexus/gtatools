/*
 * IDEStaticObject.h
 *
 *  Created on: 09.02.2010
 *      Author: alemariusnexus
 */

#ifndef IDESTATICOBJECT_H_
#define IDESTATICOBJECT_H_

#include "../gf_config.h"
#include "IDEEntity.h"

class IDEStaticObject : public IDEEntity
{
public:
	IDEStaticObject(int32_t id, char* modelName, char* textureName, int32_t numSubObjects,
			float* drawDistances, int32_t flags);


    float *getDrawDistances() const
    {
        return drawDistances;
    }

    int32_t getFlags() const
    {
        return flags;
    }

    int32_t getId() const
    {
        return id;
    }

    char *getModelName() const
    {
        return modelName;
    }

    int32_t getNumSubObjects() const
    {
        return numSubObjects;
    }

    char *getTextureName() const
    {
        return textureName;
    }

private:
	int32_t id;
	char* modelName;
	char* textureName;
	int32_t numSubObjects;
	float* drawDistances;
	int32_t flags;
};

#endif /* IDESTATICOBJECT_H_ */
