/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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
 */

#ifndef IDEPEDESTRIAN_H_
#define IDEPEDESTRIAN_H_

#include "../config.h"
#include "IDEEntity.h"




class IDEPedestrian : public IDEEntity {
public:
	IDEPedestrian(int32_t id, const char* modelName, const char* txdName, const char* defaultPedType,
			const char* behavior, const char* animGroup, int32_t drivableCarClasses, int32_t flags,
			const char* secondaryAnimFile, int32_t preferredRadio1, int32_t preferredRadio2,
			const char* voiceFile, const char* voice1, const char* voice2);
	virtual ~IDEPedestrian();

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

	virtual idetype_t getType() const { return IDETypePedestrian; }

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
