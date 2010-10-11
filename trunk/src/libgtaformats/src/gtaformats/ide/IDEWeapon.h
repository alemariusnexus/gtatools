/*
 * IDEWeapon.h
 *
 *  Created on: 11.10.2010
 *      Author: alemariusnexus
 */

#ifndef IDEWEAPON_H_
#define IDEWEAPON_H_

#include "IDEEntity.h"

#define IDE_TYPE_WEAPON 4



class IDEWeapon : public IDEEntity {
public:
	IDEWeapon(int32_t id, const char* modelName, const char* txdName, const char* animName,
			int32_t objectCount, float* drawDists, int32_t flags);

	const char* getModelName() const { return modelName; }
	const char* getTXDName() const { return txdName; }
	const char* getAnimationName() const { return animName; }
	int32_t getObjectCount() const { return objectCount; }
	const float* getDrawDistances() const { return drawDists; }
	int32_t getFlags() const { return flags; }

	virtual idetype_t getType() const { return IDE_TYPE_WEAPON; }

private:
	char* modelName;
	char* txdName;
	char* animName;
	int32_t objectCount;
	float* drawDists;
	int32_t flags;
};

#endif /* IDEWEAPON_H_ */
