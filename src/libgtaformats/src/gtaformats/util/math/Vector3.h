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

#ifndef VECTOR3_H_
#define VECTOR3_H_

#include <gtaformats/config.h>
#include <cstring>
#include "Vector4.h"


class Vector3
{
public:
	static const Vector3 Zero, One;
	static const Vector3 UnitX, UnitY, UnitZ;
	static const Vector3 NegativeUnitX, NegativeUnitY, NegativeUnitZ;

private:
	friend class Vector4;

public:
	Vector3() { data[0] = 0.0f; data[1] = 0.0f; data[2] = 0.0f; }
	Vector3(const Vector3& other) { memcpy(data, other.data, 3*4); }
	Vector3(const Vector4& other) { memcpy(data, other.data.f, 3*4); }
	Vector3(float* data) { memcpy(this->data, data, 3*4); }
	Vector3(float x, float y, float z) { data[0] = x; data[1] = y; data[2] = z; }
	const float* toArray() const { return data; }
	float* toArray() { return data; }
	float getX() const { return data[0]; }
	float getY() const { return data[1]; }
	float getZ() const { return data[2]; }
	void setX(float x) { data[0] = x; }
	void setY(float y) { data[1] = y; }
	void setZ(float z) { data[2] = z; }
	float operator[](unsigned int index) const { return data[index]; }
	void mirrorYZ() { float y = data[1]; data[1] = data[2]; data[2] = y; }
	void scale(float x, float y, float z) { data[0] *= x; data[1] *= y; data[2] *= z; }
	void scale(float f) { data[0] *= f; data[1] *= f; data[2] *= f; }
	Vector3& operator=(const Vector3& rhv);
	Vector3& operator+=(const Vector3& rhv);
	const Vector3 operator+(const Vector3& rhv) const { return Vector3(*this) += rhv; }
	Vector3& operator-=(const Vector3& rhv);
	const Vector3 operator-(const Vector3& rhv) const { return Vector3(*this) -= rhv; }
	Vector3& operator*=(float rhv);
	const Vector3 operator*(float rhv) const { return Vector3(*this) *= rhv; }
	float operator*(const Vector3& rhv) const { return dot(rhv); }
	const Vector3 operator-() const;
	float dot(const Vector3& rhv) const;
	const Vector3 cross(const Vector3& rhv) const;
	float length() const;
	float normalize();
	float angle(const Vector3& other) const;

private:
	float data[3];
};

#endif /* VECTOR3_H_ */
