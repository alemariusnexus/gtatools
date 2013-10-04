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

#include "Vector4.h"
#include "Vector3.h"
#include <cmath>


const Vector4 Vector4::Zero = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
const Vector4 Vector4::One = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
const Vector4 Vector4::UnitX = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
const Vector4 Vector4::UnitY = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
const Vector4 Vector4::UnitZ = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
const Vector4 Vector4::NegativeUnitX = Vector4(-1.0f, 0.0f, 0.0f, 1.0f);
const Vector4 Vector4::NegativeUnitY = Vector4(0.0f, -1.0f, 0.0f, 1.0f);
const Vector4 Vector4::NegativeUnitZ = Vector4(0.0f, 0.0f, -1.0f, 1.0f);




Vector4::Vector4(const Vector3& other, float w)
{
	//memcpy(data.f, other.data, 3*4);
	memcpy(data.f, other.data, 3*4);
	data.f[3] = w;
}


Vector4& Vector4::operator=(const Vector4& rhv)
{
	if (this != &rhv) {
		memcpy(data.f, rhv.data.f, sizeof(data));
	}

	return *this;
}


Vector4& Vector4::operator+=(const Vector4& rhv)
{
#ifdef __GNUCBLA__
	data.v += rhv.data.v;
#else
	data.f[0] += rhv.data.f[0];
	data.f[1] += rhv.data.f[1];
	data.f[2] += rhv.data.f[2];
	data.f[3] += rhv.data.f[3];
#endif
	return *this;
}


Vector4& Vector4::operator-=(const Vector4& rhv)
{
#ifdef __GNUCBLA__
	data.v -= rhv.data.v;
#else
	data.f[0] -= rhv.data.f[0];
	data.f[1] -= rhv.data.f[1];
	data.f[2] -= rhv.data.f[2];
	data.f[3] -= rhv.data.f[3];
#endif
	return *this;
}


Vector4& Vector4::operator*=(float rhv)
{
#ifdef __GNUCBLA__
	Vec4SF rhvv;
	rhvv.f[0] = rhv;
	rhvv.f[1] = rhv;
	rhvv.f[2] = rhv;
	rhvv.f[3] = rhv;
	data.v *= rhvv.v;
#else
	data.f[0] *= rhv;
	data.f[1] *= rhv;
	data.f[2] *= rhv;
	data.f[3] *= rhv;
#endif

	return *this;
}


const Vector4 Vector4::operator-() const
{
	return Vector4(-data.f[0], -data.f[1], -data.f[2], -data.f[3]);
}


float Vector4::dot(const Vector4& rhv) const
{
#ifdef __GNUCBLA__
	Vec4SF res;
	res.v = data.v*rhv.data.v;

	return res.f[0] + res.f[1] + res.f[2] + res.f[3];
#else
	return data.f[0]*rhv.data.f[0] + data.f[1]*rhv.data.f[1] + data.f[2]*rhv.data.f[2] + data.f[3]*rhv.data.f[3];
#endif
}


#ifdef GNUC_DEFINED
#define __GNUCBLA__
#endif
