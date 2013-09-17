/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#include "IPLInstance.h"
#include <cstring>



IPLInstance::IPLInstance (
		int32_t id, const CString& modelName,
		float posX, float posY, float posZ,
		float rotX, float rotY, float rotZ, float rotW,
		float scaleX, float scaleY, float scaleZ,
		int32_t interior,
		int32_t lod
) : id(id), modelName(modelName),
	posX(posX), posY(posY), posZ(posZ),
	rotX(rotX), rotY(rotY), rotZ(rotZ), rotW(rotW),
	scaleX(scaleX), scaleY(scaleY), scaleZ(scaleZ),
	interior(interior),
	lod(lod)
{
}


IPLInstance::~IPLInstance()
{
}

