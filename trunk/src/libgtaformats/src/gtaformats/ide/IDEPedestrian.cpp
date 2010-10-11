/*
 * IDEPedestrian.cpp
 *
 *  Created on: 11.10.2010
 *      Author: alemariusnexus
 */

#include "IDEPedestrian.h"
#include <cstring>



IDEPedestrian::IDEPedestrian(int32_t id, const char* modelName, const char* txdName,
			const char* defaultPedType, const char* behavior, const char* animGroup,
			int32_t drivableCarClasses, int32_t flags, const char* secondaryAnimFile, int32_t preferredRadio1,
			int32_t preferredRadio2, const char* voiceFile, const char* voice1, const char* voice2)
		: IDEEntity(id), modelName(new char[strlen(modelName)+1]), txdName(new char[strlen(txdName)+1]),
		  defaultPedType(new char[strlen(defaultPedType)+1]), behavior(new char[strlen(behavior)+1]),
		  animGroup(new char[strlen(animGroup)+1]), drivableCarClasses(drivableCarClasses), flags(flags),
		  secondaryAnimFile(secondaryAnimFile ? new char[strlen(secondaryAnimFile)+1] : NULL),
		  preferredRadio1(preferredRadio1), preferredRadio2(preferredRadio2),
		  voiceFile(voiceFile ? new char[strlen(voiceFile)+1] : NULL),
		  voice1(voice1 ? new char[strlen(voice1)+1] : NULL),
		  voice2(voice2 ? new char[strlen(voice2)+1] : NULL)
{
	strcpy(this->modelName, modelName);
	strcpy(this->txdName, txdName);
	strcpy(this->defaultPedType, defaultPedType);
	strcpy(this->behavior, behavior);
	strcpy(this->animGroup, animGroup);

	if (secondaryAnimFile)
		strcpy(this->secondaryAnimFile, secondaryAnimFile);
	if (voiceFile)
		strcpy(this->voiceFile, voiceFile);
	if (voice1)
		strcpy(this->voice1, voice1);
	if (voice2)
		strcpy(this->voice2, voice2);
}


IDEPedestrian::~IDEPedestrian()
{
	delete[] modelName;
	delete[] txdName;
	delete[] defaultPedType;
	delete[] behavior;
	delete[] animGroup;

	if (secondaryAnimFile)
		delete[] secondaryAnimFile;
	if (voiceFile)
		delete[] voiceFile;
	if (voice1)
		delete[] voice1;
	if (voice2)
		delete[] voice2;
}