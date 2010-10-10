/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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

#include "Vector4.h"
#include "Vector3.h"
#include <cmath>


//const Vector4 Vector4::ZERO = Vector4();



Vector4::Vector4(const Vector3& other)
{
	memcpy(data.f, other.data, 3*4);
	data.f[3] = 1.0f;
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
	data.v += rhv.data.v;
	return *this;
}


Vector4& Vector4::operator-=(const Vector4& rhv)
{
	data.v += rhv.data.v;
	return *this;
}


Vector4& Vector4::operator*=(float rhv)
{
	Vec4SF rhvv = {rhv, rhv, rhv, rhv};
	data.v *= rhvv.v;
	return *this;
}


const Vector4 Vector4::operator-() const
{
	return Vector4(-data.f[0], -data.f[1], -data.f[2], -data.f[3]);
}


float Vector4::dot(const Vector4& rhv) const
{
	Vec4SF res;
	res.v = data.v*rhv.data.v;

	return res.f[0] + res.f[1] + res.f[2] + res.f[3];
}
