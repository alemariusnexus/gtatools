/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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

#ifndef IDETIMEDOBJECT_H_
#define IDETIMEDOBJECT_H_

#include <gf_config.h>
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
