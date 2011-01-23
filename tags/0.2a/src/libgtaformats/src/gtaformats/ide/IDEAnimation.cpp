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

#include "IDEAnimation.h"
#include <cstring>

IDEAnimation::IDEAnimation(int32_t id, const char* modelName, const char* texName, const char* animName,
			float drawDist, int32_t flags)
		: IDEEntity(id), modelName(new char[strlen(modelName)+1]), texName(new char[strlen(texName)+1]),
		  animName(new char[strlen(animName)+1]), drawDist(drawDist), flags(flags)
{
	strcpy(this->modelName, modelName);
	strcpy(this->texName, texName);
	strcpy(this->animName, animName);
}


IDEAnimation::~IDEAnimation()
{
	delete[] modelName;
	delete[] texName;
	delete[] animName;
}