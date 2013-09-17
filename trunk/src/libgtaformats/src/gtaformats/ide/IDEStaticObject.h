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

#ifndef IDESTATICOBJECT_H_
#define IDESTATICOBJECT_H_

#include <gtaformats/config.h>
#include "../util/CString.h"
#include "IDEEntity.h"



class IDEStaticObject : public IDEEntity
{
public:
	enum Flags
	{
		WetEffect = (1 << 0),
		NightObject = (1 << 1),
		AlphaTransparency1 = (1 << 2),
		AlphaTransparency2 = (1 << 3),
		DayObject = (1 << 4),
		InteriorObject = (1 << 5),
		DisableShadowMesh = (1 << 6),
		DisableCulling = (1 << 7),
		DisableDrawDistance = (1 << 8),
		Breakable = (1 << 9),
		StagedBreakable = (1 << 10),
		GarageDoor = (1 << 11),
		Multiclump = (1 << 12),
		Unknown1 = (1 << 13),
		Unknown2 = (1 << 14),
		WeatherBrightness = (1 << 15),
		Explosive = (1 << 16),
		Unknown3 = (1 << 17),
		Unknown4 = (1 << 18),
		Unknown5 = (1 << 19),
		GraffityClumpSwitch = (1 << 20),
		DisableBackfaceCulling = (1 << 21),
		Unknown6 = (1 << 22)
	};

public:
	IDEStaticObject(int32_t id, const CString& modelName, const CString& txdName, int32_t numSubObjects,
			float* drawDistances, int32_t flags);
	virtual ~IDEStaticObject();


    float *getDrawDistances() const { return drawDistances; }

    int32_t getFlags() const { return flags; }

    CString getModelName() const { return modelName; }

    int32_t getNumSubObjects() const { return numSubObjects; }

    CString getTXDArchiveName() const { return txdName; }

    virtual idetype_t getType() const { return IDETypeStaticObject; }

private:
	CString modelName;
	CString txdName;
	int32_t numSubObjects;
	float* drawDistances;
	int32_t flags;
};

#endif /* IDESTATICOBJECT_H_ */
