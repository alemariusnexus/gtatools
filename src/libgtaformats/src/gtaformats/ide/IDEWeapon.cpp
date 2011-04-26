/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#include "IDEWeapon.h"
#include <cstring>



IDEWeapon::IDEWeapon(int32_t id, const char* modelName, const char* txdName, const char* animName,
			int32_t objectCount, float* drawDists, int32_t flags)
		: IDEEntity(id), modelName(new char[strlen(modelName)+1]), txdName(new char[strlen(txdName)+1]),
		  animName(new char[strlen(animName)+1]), objectCount(objectCount), drawDists(drawDists),
		  flags(flags)
{
	strcpy(this->modelName, modelName);
	strcpy(this->txdName, txdName);
	strcpy(this->animName, animName);
}


IDEWeapon::~IDEWeapon()
{
	delete[] modelName;
	delete[] txdName;
	delete[] animName;
	delete[] drawDists;
}
