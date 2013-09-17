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

#ifndef IPLCAR_H_
#define IPLCAR_H_

#include <gtaformats/config.h>
#include "IPLStatement.h"

#define IPL_TYPE_CAR 1


class IPLCar : public IPLStatement {
public:
	IPLCar(float posX, float posY, float posZ, float angle, int32_t carId, int32_t primaryColor,
			int32_t secondaryColor, bool forceSpawn, int8_t alarmProbability, int8_t doorLockProbability,
			int32_t unknown1 = 0, int32_t unknown2 = 0);

	virtual ipltype_t getType() const { return IPL_TYPE_CAR; }
	void getPosition(float& x, float& y, float& z) const { x = posX; y = posY; z = posZ; }
	float getAngle() const { return angle; }
	int32_t getCarID() const { return carId; }
	int32_t getPrimaryColor() const { return primaryColor; }
	int32_t getSecondaryColor() const { return secondaryColor; }
	bool isForceSpawn() const { return forceSpawn; }
	int8_t getAlarmProbability() const { return alarmProbability; }
	int8_t getDoorLockProbability() const { return doorLockProbability; }
	int32_t getUnknown1() const { return unknown1; }
	int32_t getUnknown2() const { return unknown2; }

private:
	float posX, posY, posZ;
	float angle;
	int32_t carId;
	int32_t primaryColor, secondaryColor;
	bool forceSpawn;
	int8_t alarmProbability;
	int8_t doorLockProbability;
	int32_t unknown1, unknown2;
};

#endif /* IPLCAR_H_ */
