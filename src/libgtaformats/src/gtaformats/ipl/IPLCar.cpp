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

#include "IPLCar.h"



IPLCar::IPLCar(	float posX, float posY, float posZ, float angle, int32_t carId, int32_t primaryColor,
				int32_t secondaryColor, bool forceSpawn, int8_t alarmProbability, int8_t doorLockProbability,
				int32_t unknown1, int32_t unknown2)
		: posX(posX), posY(posY), posZ(posZ), angle(angle), carId(carId), primaryColor(primaryColor),
		  secondaryColor(secondaryColor), forceSpawn(forceSpawn), alarmProbability(alarmProbability),
		  doorLockProbability(doorLockProbability), unknown1(unknown1), unknown2(unknown2)
{
}
