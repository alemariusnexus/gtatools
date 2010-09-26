/*
 * IPLCar.h
 *
 *  Created on: 26.09.2010
 *      Author: alemariusnexus
 */

#ifndef IPLCAR_H_
#define IPLCAR_H_

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
