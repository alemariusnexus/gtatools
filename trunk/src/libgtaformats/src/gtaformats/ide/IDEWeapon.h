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

#ifndef IDEWEAPON_H_
#define IDEWEAPON_H_

#include <gtaformats/config.h>
#include "../util/CString.h"
#include "IDEEntity.h"



class IDEWeapon : public IDEEntity {
public:
	IDEWeapon(int32_t id, const CString& modelName, const CString& txdName, const CString& animName,
			int32_t objectCount, float* drawDists, int32_t flags);
	virtual ~IDEWeapon();

	CString getModelName() const { return modelName; }
	CString getTXDArchiveName() const { return txdName; }
	CString getAnimationName() const { return animName; }
	int32_t getObjectCount() const { return objectCount; }
	const float* getDrawDistances() const { return drawDists; }
	int32_t getFlags() const { return flags; }

	virtual idetype_t getType() const { return IDETypeWeapon; }

private:
	CString modelName;
	CString txdName;
	CString animName;
	int32_t objectCount;
	float* drawDists;
	int32_t flags;
};

#endif /* IDEWEAPON_H_ */
