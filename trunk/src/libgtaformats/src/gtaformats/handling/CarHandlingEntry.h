/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#ifndef CARHANDLINGENTRY_H_
#define CARHANDLINGENTRY_H_

#include "HandlingEntry.h"
#include <gtaformats/config.h>
#include <nxcommon/CString.h>
#include <nxcommon/math/Vector3.h>



class CarHandlingEntry : public HandlingEntry
{
public:
	enum EngineDriveType
	{
		FrontDrive,
		RearDrive,
		FourWheelDrive
	};

	enum EngineType
	{
		PetrolEngine,
		DieselEngine,
		ElectricEngine
	};

	enum Level
	{
		Low,
		Medium,
		High
	};

	enum ModelFlags
	{
		ModelFlagIsVan = 0x00000001,
		ModelFlagIsBus = 0x00000002,
		ModelFlagIsLow = 0x00000004,
		ModelFlagIsBig = 0x00000008,
		ModelFlagReverseBonnet = 0x00000010,
		ModelFlagHangingBoot = 0x00000020,
		ModelFlagTailgateBoot = 0x00000040,
		ModelFlagNoSwingBoot = 0x00000080,
		ModelFlagNoDoors = 0x00000100,
		ModelFlagTandemSeats = 0x00000200,
		ModelFlagSitInBoat = 0x00000400,
		ModelFlagConvertible = 0x00000800,
		ModelFlagNoExhaust = 0x00001000,
		ModelFlagDoubleExhaust = 0x00002000,
		ModelFlagNo1FPSLookBehind = 0x00004000,
		ModelFlagForceDoorCheck = 0x00008000,
		ModelFlagAxleFNoTilt = 0x00010000,
		ModelFlagAxleFSolid = 0x00020000,
		ModelFlagAxleFMcPherson = 0x00040000,
		ModelFlagAxleFReverse = 0x00080000,
		ModelFlagAxleRNoTilt = 0x00100000,
		ModelFlagAxleRSolid = 0x00200000,
		ModelFlagAxleRMcPherson = 0x00400000,
		ModelFlagAxleRReverse = 0x00800000,
		ModelFlagIsBike = 0x01000000,
		ModelFlagIsHeli = 0x02000000,
		ModelFlagIsPlane = 0x04000000,
		ModelFlagIsBoat = 0x08000000,
		ModelFlagBouncePanels = 0x10000000,
		ModelFlagDoubleRWheels = 0x20000000,
		ModelFlagForceGroundClearance = 0x40000000,
		ModelFlagIsHatchback = 0x80000000,
	};

	enum HandlingFlags
	{
		HandlingFlag1GBoost = 0x00000001,
		HandlingFlag2GBoost = 0x00000002,
		HandlingFlagNPCAntiRoll = 0x00000004,
		HandlingFlagNPCNeutralHandle = 0x00000008,
		HandlingFlagNoHandbrake = 0x00000010,
		HandlingFlagSteerRearWheels = 0x00000020,
		HandlingFlagHBRearWheelSteer = 0x00000040,
		HandlingFlagAltSteerOpt = 0x00000080,
		HandlingFlagWheelFNarrow2 = 0x00000100,
		HandlingFlagWheelFNarrow = 0x00000200,
		HandlingFlagWheelFWide = 0x00000400,
		HandlingFlagWheelFWide2 = 0x00000800,
		HandlingFlagWheelRNarrow2 = 0x00001000,
		HandlingFlagWheelRNarrow = 0x00002000,
		HandlingFlagWheelRWide = 0x00004000,
		HandlingFlagWheelRWide2 = 0x00008000,
		HandlingFlagHydraulicGeometry = 0x00010000,
		HandlingFlagHydraulicInst = 0x00020000,
		HandlingFlagHydraulicNone = 0x00040000,
		HandlingFlagNOSInst = 0x00080000,
		HandlingFlagOffroadAbility = 0x00100000,
		HandlingFlagOffroadAbility2 = 0x00200000,
		HandlingFlagHalogenLights = 0x00400000,
		HandlingFlagProcRearwheelFirst = 0x00800000,
		HandlingFlagUseMaxSPLimit = 0x01000000,
		HandlingFlagLowRider = 0x02000000,
		HandlingFlagStreetRacer = 0x04000000,
		HandlingFlagSwingingChassis = 0x10000000,
	};

	enum LightType
	{
		Long = 0,
		Small = 1,
		Big = 2,
		Tall = 3
	};

public:
	bool hasABS() const
	{
		return absInstalled;
	}

	void setHasABS(bool absInstalled)
	{
		this->absInstalled = absInstalled;
	}

	uint32_t getAnimationGroup() const
	{
		return animGroup;
	}

	void setAnimationGroup(uint32_t animGroup)
	{
		this->animGroup = animGroup;
	}

	float getBrakeBias() const
	{
		return brakeBias;
	}

	void setBrakeBias(float brakeBias)
	{
		this->brakeBias = brakeBias;
	}

	float getBrakeDeceleration() const
	{
		return brakeDeceleration;
	}

	void setBrakeDeceleration(float brakeDeceleration)
	{
		this->brakeDeceleration = brakeDeceleration;
	}

	const Vector3& getCenterOfMass() const
	{
		return centerOfMass;
	}

	void setCenterOfMass(const Vector3& centerOfMass)
	{
		this->centerOfMass = centerOfMass;
	}

	float getCollisionDamageMultiplier() const
	{
		return collisionDamageMultiplier;
	}

	void setCollisionDamageMultiplier(float collisionDamageMultiplier)
	{
		this->collisionDamageMultiplier = collisionDamageMultiplier;
	}

	float getDragMultiplier() const
	{
		return dragMultiplier;
	}

	void setDragMultiplier(float dragMultiplier)
	{
		this->dragMultiplier = dragMultiplier;
	}

	EngineDriveType getDriveType() const
	{
		return driveType;
	}

	void setDriveType(EngineDriveType driveType)
	{
		this->driveType = driveType;
	}

	float getEngineAcceleration() const
	{
		return engineAcceleration;
	}

	void setEngineAcceleration(float engineAcceleration)
	{
		this->engineAcceleration = engineAcceleration;
	}

	float getEngineInertia() const
	{
		return engineInertia;
	}

	void setEngineInertia(float engineInertia)
	{
		this->engineInertia = engineInertia;
	}

	EngineType getEngineType() const
	{
		return engineType;
	}

	void setEngineType(EngineType engineType)
	{
		this->engineType = engineType;
	}

	LightType getFrontLightType() const
	{
		return frontLightType;
	}

	void setFrontLightType(LightType frontLightType)
	{
		this->frontLightType = frontLightType;
	}

	uint32_t getHandlingFlags() const
	{
		return handlingFlags;
	}

	void setHandlingFlags(uint32_t handlingFlags)
	{
		this->handlingFlags = handlingFlags;
	}

	const CString& getID() const
	{
		return id;
	}

	void setID(const CString& id)
	{
		this->id = id;
	}

	float getMass() const
	{
		return mass;
	}

	void setMass(float mass)
	{
		this->mass = mass;
	}

	float getMaxVelocity() const
	{
		return maxVelocity;
	}

	void setMaxVelocity(float maxVelocity)
	{
		this->maxVelocity = maxVelocity;
	}

	uint32_t getModelFlags() const
	{
		return modelFlags;
	}

	void setModelFlags(uint32_t modelFlags)
	{
		this->modelFlags = modelFlags;
	}

	uint32_t getMonetaryValue() const
	{
		return monetaryValue;
	}

	void setMonetaryValue(uint32_t monetaryValue)
	{
		this->monetaryValue = monetaryValue;
	}

	uint8_t getNumGears() const
	{
		return numGears;
	}

	void setNumGears(uint8_t numGears)
	{
		this->numGears = numGears;
	}

	uint8_t getPercentSubmerged() const
	{
		return percentSubmerged;
	}

	void setPercentSubmerged(uint8_t percentSubmerged)
	{
		this->percentSubmerged = percentSubmerged;
	}

	LightType getRearLightType() const
	{
		return rearLightType;
	}

	void setRearLightType(LightType rearLightType)
	{
		this->rearLightType = rearLightType;
	}

	float getSeatOffsetDistance() const
	{
		return seatOffsetDistance;
	}

	void setSeatOffsetDistance(float seatOffsetDistance)
	{
		this->seatOffsetDistance = seatOffsetDistance;
	}

	float getSteeringLock() const
	{
		return steeringLock;
	}

	void setSteeringLock(float steeringLock)
	{
		this->steeringLock = steeringLock;
	}

	float getSuspensionAntiDiveMultiplier() const
	{
		return suspensionAntiDiveMultiplier;
	}

	void setSuspensionAntiDiveMultiplier(float suspensionAntiDiveMultiplier)
	{
		this->suspensionAntiDiveMultiplier = suspensionAntiDiveMultiplier;
	}

	float getSuspensionBias() const
	{
		return suspensionBias;
	}

	void setSuspensionBias(float suspensionBias)
	{
		this->suspensionBias = suspensionBias;
	}

	Level getSuspensionDampingLevel() const
	{
		return suspensionDampingLevel;
	}

	void setSuspensionDampingLevel(Level suspensionDampingLevel)
	{
		this->suspensionDampingLevel = suspensionDampingLevel;
	}

	Level getSuspensionForceLevel() const
	{
		return suspensionForceLevel;
	}

	void setSuspensionForceLevel(Level suspensionForceLevel)
	{
		this->suspensionForceLevel = suspensionForceLevel;
	}

	float getSuspensionHighSpeedComDamping() const
	{
		return suspensionHighSpeedComDamping;
	}

	void setSuspensionHighSpeedComDamping(float suspensionHighSpeedComDamping)
	{
		this->suspensionHighSpeedComDamping = suspensionHighSpeedComDamping;
	}

	float getSuspensionLowerLimit() const
	{
		return suspensionLowerLimit;
	}

	void setSuspensionLowerLimit(float suspensionLowerLimit)
	{
		this->suspensionLowerLimit = suspensionLowerLimit;
	}

	float getSuspensionUpperLimit() const
	{
		return suspensionUpperLimit;
	}

	void setSuspensionUpperLimit(float suspensionUpperLimit)
	{
		this->suspensionUpperLimit = suspensionUpperLimit;
	}

	float getTractionBias() const
	{
		return tractionBias;
	}

	void setTractionBias(float tractionBias)
	{
		this->tractionBias = tractionBias;
	}

	float getTractionLoss() const
	{
		return tractionLoss;
	}

	void setTractionLoss(float tractionLoss)
	{
		this->tractionLoss = tractionLoss;
	}

	float getTractionMultiplier() const
	{
		return tractionMultiplier;
	}

	void setTractionMultiplier(float tractionMultiplier)
	{
		this->tractionMultiplier = tractionMultiplier;
	}

	float getTurnMass() const
	{
		return turnMass;
	}

	void setTurnMass(float turnMass)
	{
		this->turnMass = turnMass;
	}

private:
	CString id;
	float mass;
	float turnMass;
	float dragMultiplier;
	Vector3 centerOfMass;
	uint8_t percentSubmerged;
	float tractionMultiplier;
	float tractionLoss;
	float tractionBias;
	uint8_t numGears;
	float maxVelocity;
	float engineAcceleration;
	float engineInertia;
	EngineDriveType driveType;
	EngineType engineType;
	float brakeDeceleration;
	float brakeBias;
	bool absInstalled;
	float steeringLock;
	Level suspensionForceLevel;
	Level suspensionDampingLevel;
	float suspensionHighSpeedComDamping;
	float suspensionUpperLimit;
	float suspensionLowerLimit;
	float suspensionBias;
	float suspensionAntiDiveMultiplier;
	float seatOffsetDistance;
	float collisionDamageMultiplier;
	uint32_t monetaryValue;
	uint32_t modelFlags;
	uint32_t handlingFlags;
	LightType frontLightType;
	LightType rearLightType;
	uint32_t animGroup;
};

#endif /* CARHANDLINGENTRY_H_ */
