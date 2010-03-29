/*
	Copyright 2010 David Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.
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