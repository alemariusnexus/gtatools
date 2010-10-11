/*
 * IDEWeapon.cpp
 *
 *  Created on: 11.10.2010
 *      Author: alemariusnexus
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
