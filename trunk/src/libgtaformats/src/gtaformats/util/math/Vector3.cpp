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

#include "Vector3.h"
#include <cmath>
#include <cstdio>


const Vector3 Vector3::Zero = Vector3(0.0f, 0.0f, 0.0f);
const Vector3 Vector3::One = Vector3(1.0f, 1.0f, 1.0f);
const Vector3 Vector3::UnitX = Vector3(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::UnitY = Vector3(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::UnitZ = Vector3(0.0f, 0.0f, 1.0f);
const Vector3 Vector3::NegativeUnitX = Vector3(-1.0f, 0.0f, 0.0f);
const Vector3 Vector3::NegativeUnitY = Vector3(0.0f, -1.0f, 0.0f);
const Vector3 Vector3::NegativeUnitZ = Vector3(0.0f, 0.0f, -1.0f);





Vector3& Vector3::operator=(const Vector3& rhv)
{
	if (this != &rhv) {
		memcpy(data, rhv.data, sizeof(data));
	}

	return *this;
}


Vector3& Vector3::operator+=(const Vector3& rhv)
{
	data[0] += rhv.data[0];
	data[1] += rhv.data[1];
	data[2] += rhv.data[2];
	return *this;
}


Vector3& Vector3::operator-=(const Vector3& rhv)
{
	data[0] -= rhv.data[0];
	data[1] -= rhv.data[1];
	data[2] -= rhv.data[2];
	return *this;
}


Vector3& Vector3::operator*=(float rhv)
{
	data[0] *= rhv;
	data[1] *= rhv;
	data[2] *= rhv;
	return *this;
}


const Vector3 Vector3::operator-() const
{
	return Vector3(-data[0], -data[1], -data[2]);
}


float Vector3::dot(const Vector3& rhv) const
{
	return data[0]*rhv.data[0] + data[1]*rhv.data[1] + data[2]*rhv.data[2];
}


const Vector3 Vector3::cross(const Vector3& rhv) const
{
	return Vector3 (
			data[1]*rhv[2] - data[2]*rhv[1],
			data[2]*rhv[0] - data[0]*rhv[2],
			data[0]*rhv[1] - data[1]*rhv[0]
	);
}


float Vector3::length() const
{
	double d0 = data[0];
	double d1 = data[1];
	double d2 = data[2];
	return (float) sqrt(d0*d0 + d1*d1 + d2*d2);
	//return sqrtf(this->dot(*this));
}


float Vector3::normalize()
{
	//float len = length();
	double d0 = data[0];
	double d1 = data[1];
	double d2 = data[2];
	double len = sqrt(d0*d0 + d1*d1 + d2*d2);

	//data[0] /= len;
	//data[1] /= len;
	//data[2] /= len;

	data[0] = d0 / len;
	data[1] = d1 / len;
	data[2] = d2 / len;

	return len;
}


float Vector3::angle(const Vector3& other) const
{
	float d = dot(other);

	if (d > 1.0f)
		d = 1.0f;
	else if (d < -1.0f)
		d = -1.0f;

	return acosf(d);
}
