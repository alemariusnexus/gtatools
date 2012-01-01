/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

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

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
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

