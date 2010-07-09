/*
 * IPLInstance.cpp
 *
 *  Created on: 18.04.2010
 *      Author: alemariusnexus
 */

#include "IPLInstance.h"
#include <cstring>



IPLInstance::IPLInstance (
		int32_t id, const char* modelName,
		float posX, float posY, float posZ,
		float rotX, float rotY, float rotZ, float rotW,
		float scaleX, float scaleY, float scaleZ,
		int32_t interior,
		int32_t lod
) : id(id), modelName(new char[strlen(modelName)+1]),
	posX(posX), posY(posY), posZ(posZ),
	rotX(rotX), rotY(rotY), rotZ(rotZ), rotW(rotW),
	scaleX(scaleX), scaleY(scaleY), scaleZ(scaleZ),
	interior(interior),
	lod(lod)
{
	strcpy(this->modelName, modelName);
}


IPLInstance::~IPLInstance()
{
	delete[] modelName;
}

