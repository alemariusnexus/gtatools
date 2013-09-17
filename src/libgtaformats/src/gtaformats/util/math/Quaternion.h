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

#ifndef QUATERNION_H_
#define QUATERNION_H_

#include "Vector3.h"
#include "Matrix3.h"
#include <cstdlib>
#include <cmath>
#include <cstring>



/**	\brief Mathematical Quaternion.
 *
 * 	This class represents a quaternion of the form w + ix + jy + kz.
 */
class Quaternion {
public:
	static Quaternion fromAxisAngle(const Vector3& axis, float angle);

public:
	Quaternion() {}
	Quaternion(float w, float x, float y, float z) { data[0] = w; data[1] = x; data[2] = y; data[3] = z; }
	Quaternion(const float* data) { memcpy(this->data, data, 4*sizeof(float)); }
	Quaternion(const Quaternion& other) { memcpy(data, other.data, 4*sizeof(float)); }
	float getW() const { return data[0]; }
	float getX() const { return data[1]; }
	float getY() const { return data[2]; }
	float getZ() const { return data[3]; }
	Quaternion& operator+=(const Quaternion& other);
	const Quaternion operator+(const Quaternion& rhv) const { return Quaternion(*this) += rhv; }
	Quaternion& operator-=(const Quaternion& other);
	const Quaternion operator-(const Quaternion& rhv) const { return Quaternion(*this) -= rhv; }
	Quaternion& operator*=(const Quaternion& other);
	const Quaternion operator*(const Quaternion& rhv) const { return Quaternion(*this) *= rhv; }
	const Vector3 operator*(const Vector3& rhv) const;
	Quaternion& operator*=(float s);
	const Quaternion operator*(float s) const { return Quaternion(*this) *= s; }
	const Quaternion conjugate() const { return Quaternion(data[0], -data[1], -data[2], -data[3]); }
	float lengthSquared() const
			{ return data[0]*data[0] + data[1]*data[1] + data[2]*data[2] + data[3]*data[3]; }
	float length() const
			{ return sqrtf(lengthSquared()); }
	void normalize() { float l = length(); data[0] /= l; data[1] /= l; data[2] /= l; data[3] /= l; }
	float dot(const Quaternion& rhv) const
			{ return data[0]*rhv.data[0] + data[1]*rhv.data[1] + data[2]*rhv.data[2] + data[3]*rhv.data[3]; }
	float angle(const Quaternion& other) const
			{ return acosf(fabs(dot(other)) / sqrt(lengthSquared() * other.lengthSquared())); }
	const Quaternion lerp(const Quaternion& other, float t);
	const Quaternion nlerp(const Quaternion& other, float t)
			{ Quaternion res = lerp(other, t); res.normalize(); return res; }
	const Quaternion slerp(const Quaternion& other, float t) const;
	const Matrix3 toMatrix() const;
	void toAxisAngle(Vector3& axis, float& angle, const Vector3& fallbackAxis = Vector3::UnitZ);

private:
	float data[4];
};

#endif /* QUATERNION_H_ */
