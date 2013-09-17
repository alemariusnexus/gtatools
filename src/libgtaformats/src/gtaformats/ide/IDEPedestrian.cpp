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

#include "IDEPedestrian.h"
#include <cstring>
#include <cstdio>



IDEPedestrian::IDEPedestrian(int32_t id, const CString& modelName, const CString& txdName,
			const CString& defaultPedType, const CString& behavior, const CString& animGroup,
			int32_t drivableCarClasses, int32_t flags, const CString& secondaryAnimFile, int32_t preferredRadio1,
			int32_t preferredRadio2, const CString& voiceFile, const CString& voice1, const CString& voice2)
		: IDEEntity(id), modelName(modelName), txdName(txdName), defaultPedType(defaultPedType),
		  behavior(behavior), animGroup(animGroup), drivableCarClasses(drivableCarClasses), flags(flags),
		  secondaryAnimFile(secondaryAnimFile),
		  preferredRadio1(preferredRadio1), preferredRadio2(preferredRadio2),
		  voiceFile(voiceFile), voice1(voice1), voice2(voice2)
{
}


IDEPedestrian::~IDEPedestrian()
{
}
