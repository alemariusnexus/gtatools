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

#ifndef IPLINSTANCE_H_
#define IPLINSTANCE_H_

#include <gtaformats/config.h>
#include "../util/CString.h"
#include "IPLStatement.h"

#define IPL_TYPE_INSTANCE 0



class IPLInstance : public IPLStatement {
public:
	IPLInstance	(
			int32_t id, const CString& modelName,
			float posX, float posY, float posZ,
			float rotX, float rotY, float rotZ, float rotW,
			float scaleX = 1.0f, float scaleY = 1.0f, float scaleZ = 1.0f,
			int32_t interior = 0,
			int32_t lod = 0
	);
	virtual ~IPLInstance();

	virtual ipltype_t getType() const { return IPL_TYPE_INSTANCE; }
	int32_t getID() const { return id; }
	CString getModelName() const { return modelName; }
	void getPosition(float& x, float& y, float& z) const { x = posX; y = posY; z = posZ; }
	void getRotation(float& x, float& y, float& z, float& w) const
			{ x = rotX; y = rotY; z = rotZ; w = rotW; }
	void getScale(float& x, float& y, float& z) const { x = scaleX; y = scaleY; z = scaleZ; }
	int32_t getInterior() const { return interior; }
	int32_t getLOD() const { return lod; }

private:
	int32_t id;
	CString modelName;
	float posX;
	float posY;
	float posZ;
	float rotX;
	float rotY;
	float rotZ;
	float rotW;
	float scaleX;
	float scaleY;
	float scaleZ;
	int32_t interior;
	int32_t lod;
};

#endif /* IPLINSTANCE_H_ */
