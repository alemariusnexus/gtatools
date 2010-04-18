/*
 * IDEAnimation.cpp
 *
 *  Created on: 09.04.2010
 *      Author: alemariusnexus
 */

#include "IDEAnimation.h"
#include <cstring>

IDEAnimation::IDEAnimation(int32_t id, const char* modelName, const char* texName, const char* animName,
			float drawDist, int32_t flags)
		: IDEEntity(id), modelName(new char[strlen(modelName)+1]), texName(new char[strlen(texName)+1]),
		  animName(new char[strlen(animName)+1]), drawDist(drawDist), flags(flags)
{
	strcpy(this->modelName, modelName);
	strcpy(this->texName, texName);
	strcpy(this->animName, animName);
}


IDEAnimation::~IDEAnimation()
{
	delete[] modelName;
	delete[] texName;
	delete[] animName;
}
