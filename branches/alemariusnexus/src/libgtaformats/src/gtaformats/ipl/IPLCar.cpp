/*
 * IPLCar.cpp
 *
 *  Created on: 26.09.2010
 *      Author: alemariusnexus
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
