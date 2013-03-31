/*
	Copyright 2010-2012 David "Alemarius Nexus" Lerch

	This file is part of gtatools-gui.

	gtatools-gui is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtatools-gui is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtatools-gui.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "TimedMapItemDefinition.h"
#include "Engine.h"
#include <gtaformats/util/util.h>



TimedMapItemDefinition::TimedMapItemDefinition(MeshPointer* meshPtr, TextureSource* texSrc,
		CollisionShapePointer* colPtr, ShadowMeshPointer* smeshPtr, float drawDist,
		int8_t timeOn, int8_t timeOff)
		: StaticMapItemDefinition(meshPtr, texSrc, colPtr, smeshPtr, drawDist), timeOn(timeOn), timeOff(timeOff)
{
}


TimedMapItemDefinition::TimedMapItemDefinition(IDETimedObject& object)
		: StaticMapItemDefinition(object), timeOn((int8_t) object.getTimeOn()),
		  timeOff((int8_t) object.getTimeOff())
{
}


bool TimedMapItemDefinition::isVisible() const
{
	int8_t timeH = Engine::getInstance()->getGameHours();

	return IsTimeBetween(timeH, 0, timeOn, 0, timeOff, 0);
}
