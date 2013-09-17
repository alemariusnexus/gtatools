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

#include "Quaternion.h"
#include <cstdio>



// This code is partially based on code from the Bullet btQuaternion class.



Quaternion Quaternion::fromAxisAngle(const Vector3& axis, float angle)
{
	angle *= 0.5f;
	float s = sinf(angle);
	return Quaternion(cosf(angle), axis.getX() * s, axis.getY() * s, axis.getZ() * s);
}


Quaternion& Quaternion::operator+=(const Quaternion& other)
{
	data[0] += other.data[0];
	data[1] += other.data[1];
	data[2] += other.data[2];
	data[3] += other.data[3];
	return *this;
}


Quaternion& Quaternion::operator-=(const Quaternion& other)
{
	data[0] -= other.data[0];
	data[1] -= other.data[1];
	data[2] -= other.data[2];
	data[3] -= other.data[3];
	return *this;
}


Quaternion& Quaternion::operator*=(const Quaternion& other)
{
	float odata[4];
	memcpy(odata, data, sizeof(data));

	data[0] = odata[0]*other.data[0] - odata[1]*other.data[1] - odata[2]*other.data[2] - odata[3]*other.data[3];
	data[1] = odata[1]*other.data[0] + odata[0]*other.data[1] + odata[2]*other.data[3] - odata[3]*other.data[2];
	data[2] = odata[0]*other.data[2] - odata[1]*other.data[3] + odata[2]*other.data[0] + odata[3]*other.data[1];
	data[3] = odata[0]*other.data[3] + odata[1]*other.data[2] - odata[2]*other.data[1] + odata[3]*other.data[0];

	return *this;
}


Quaternion& Quaternion::operator*=(float s)
{
	data[0] *= s;
	data[1] *= s;
	data[2] *= s;
	data[3] *= s;
	return *this;
}


const Vector3 Quaternion::operator*(const Vector3& rhv) const
{
	Quaternion r = *this * Quaternion(0.0f, rhv.getX(), rhv.getY(), rhv.getZ()) * conjugate();
	return Vector3(r.getX(), r.getY(), r.getZ());
}


const Quaternion Quaternion::lerp(const Quaternion& other, float t)
{
	return *this + (other - (*this)) * t;
}


const Quaternion Quaternion::slerp(const Quaternion& other, float t) const
{
	float cosTheta = fabs(dot(other)) / sqrt(lengthSquared() * other.lengthSquared());

	if (cosTheta >= 1.0f)
		return *this;

	float theta = acosf(cosTheta);
	float sinTheta = sin(theta);

	if (fabs(sinTheta) < 0.0000001f) {
		// When theta is very near to 180 degrees, slerp does not work as expected. 180 degrees in quaternion
		// math means rotation by 360 degrees. This is commonly interpreted as not rotating at all.
		return *this;
	}

	float rs = 1.0 / sinTheta;
	float f1 = sinf((1.0-t) * theta);
	float f2 = sinf(t * theta);

	if (dot(other) < 0.0f) {
		return Quaternion (
				(data[0]*f1 - other.data[0]*f2) * rs,
				(data[1]*f1 - other.data[1]*f2) * rs,
				(data[2]*f1 - other.data[2]*f2) * rs,
				(data[3]*f1 - other.data[3]*f2) * rs
		);
	} else {
		return Quaternion (
				(data[0]*f1 + other.data[0]*f2) * rs,
				(data[1]*f1 + other.data[1]*f2) * rs,
				(data[2]*f1 + other.data[2]*f2) * rs,
				(data[3]*f1 + other.data[3]*f2) * rs
		);
	}
}


const Matrix3 Quaternion::toMatrix() const
{
	float xx = data[1]*data[1];
	float yy = data[2]*data[2];
	float zz = data[3]*data[3];
	float xy = data[1]*data[2];
	float zw = data[3]*data[0];
	float xz = data[1]*data[3];
	float yw = data[2]*data[0];
	float yz = data[2]*data[3];
	float xw = data[1]*data[0];

	return Matrix3 (
			1.0f - 2.0f * (yy+zz),	2.0f * (xy+zw),			2.0f * (xz-yw),
			2.0f * (xy-zw),			1.0f - 2.0f * (xx+zz),	2.0f * (yz+xw),
			2.0f * (xz+yw),			2.0f * (yz-xw),			1.0f - 2.0f * (xx+yy)
	);
}


void Quaternion::toAxisAngle(Vector3& axis, float& angle, const Vector3& fallbackAxis)
{
	normalize();
	angle = 2.0f * acosf(data[0]);
	float sqrtInvW2 = sqrtf(1.0f - data[0]*data[0]);

	if (sqrtInvW2 < 0.0000001f) {
		axis = fallbackAxis;
		return;
	}

	axis = Vector3(data[1] / sqrtInvW2, data[2] / sqrtInvW2, data[3] / sqrtInvW2);
}


