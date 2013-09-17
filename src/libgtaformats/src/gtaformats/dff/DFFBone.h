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

#ifndef DFFBONE_H_
#define DFFBONE_H_

#include <gtaformats/config.h>
#include "../util/math/Matrix4.h"



class DFFBone {
public:
	enum BoneType
	{
		Deformable = 0,
		Nub = 1,
		Unknown1 = 2,
		Rigid = 3
	};

public:
	DFFBone(const DFFBone& other) : index(other.index), num(other.num), type(other.type) {}
	int32_t getIndex() const { return index; }
	int32_t getNumber() const { return num; }
	int32_t getType() const { return type; }
	void setIndex(int32_t idx) { index = idx; }
	Matrix4 getInverseBoneMatrix() const { return ibm; }

public:
	DFFBone() {}

private:
	int32_t index;
	int32_t num;
	int32_t type;
	Matrix4 ibm;

private:
	friend class DFFLoader;
};

#endif /* DFFBONE_H_ */
