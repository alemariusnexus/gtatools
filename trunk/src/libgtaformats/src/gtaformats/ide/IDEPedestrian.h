/*
 * IDEPedestrian.h
 *
 *  Created on: 11.10.2010
 *      Author: alemariusnexus
 */

#ifndef IDEPEDESTRIAN_H_
#define IDEPEDESTRIAN_H_

#include "IDEEntity.h"


#define IDE_TYPE_PEDESTRIAN 3



class IDEPedestrian : public IDEEntity {
public:
	IDEPedestrian(int32_t id, const char* modelName, const char* txdName, const char* defaultPedType,
			const char* behavior, const char* animGroup, int32_t drivableCarClasses, int32_t flags,
			const char* secondaryAnimFile, int32_t preferredRadio1, int32_t preferredRadio2,
			const char* voiceFile, const char* voice1, const char* voice2);
	~IDEPedestrian();

	const char* getModelName() const { return modelName; }
	const char* getTXDName() const { return txdName; }
	const char* getDefaultPedType() const { return defaultPedType; }
	const char* getBehavior() const { return behavior; }
	const char* getAnimationGroup() const { return animGroup; }
	int32_t getDrivableCarClasses() const { return drivableCarClasses; }
	int32_t getFlags() const { return flags; }
	const char* getSecondaryAnimationFile() const { return secondaryAnimFile; }
	int32_t getPreferredRadio1() const { return preferredRadio1; }
	int32_t getPreferredRadio2() const { return preferredRadio2; }
	const char* getVoiceFile() const { return voiceFile; }
	const char* getVoice1() const { return voice1; }
	const char* getVoice2() const { return voice2; }

	virtual idetype_t getType() const { return IDE_TYPE_PEDESTRIAN; }

private:
	char* modelName;
	char* txdName;
	char* defaultPedType;
	char* behavior;
	char* animGroup;
	int32_t drivableCarClasses;
	int32_t flags;
	char* secondaryAnimFile;
	int32_t preferredRadio1;
	int32_t preferredRadio2;
	char* voiceFile;
	char* voice1;
	char* voice2;
};

#endif /* IDEPEDESTRIAN_H_ */