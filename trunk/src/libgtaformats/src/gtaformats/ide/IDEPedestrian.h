/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#ifndef IDEPEDESTRIAN_H_
#define IDEPEDESTRIAN_H_

#include <gtaformats/config.h>
#include "../util/CString.h"
#include "IDEEntity.h"




class IDEPedestrian : public IDEEntity {
public:
	IDEPedestrian(int32_t id, const CString& modelName, const CString& txdName, const CString& defaultPedType,
			const CString& behavior, const CString& animGroup, int32_t drivableCarClasses, int32_t flags,
			const CString& secondaryAnimFile, int32_t preferredRadio1, int32_t preferredRadio2,
			const CString& voiceFile, const CString& voice1, const CString& voice2);
	virtual ~IDEPedestrian();

	CString getModelName() const { return modelName; }
	CString getTXDArchiveName() const { return txdName; }
	CString getDefaultPedType() const { return defaultPedType; }
	CString getBehavior() const { return behavior; }
	CString getAnimationGroup() const { return animGroup; }
	int32_t getDrivableCarClasses() const { return drivableCarClasses; }
	int32_t getFlags() const { return flags; }
	CString getSecondaryAnimationFile() const { return secondaryAnimFile; }
	int32_t getPreferredRadio1() const { return preferredRadio1; }
	int32_t getPreferredRadio2() const { return preferredRadio2; }
	CString getVoiceFile() const { return voiceFile; }
	CString getVoice1() const { return voice1; }
	CString getVoice2() const { return voice2; }

	virtual idetype_t getType() const { return IDETypePedestrian; }

private:
	CString modelName;
	CString txdName;
	CString defaultPedType;
	CString behavior;
	CString animGroup;
	int32_t drivableCarClasses;
	int32_t flags;
	CString secondaryAnimFile;
	int32_t preferredRadio1;
	int32_t preferredRadio2;
	CString voiceFile;
	CString voice1;
	CString voice2;
};

#endif /* IDEPEDESTRIAN_H_ */
