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

#include "Matrix3.h"
#include <cstring>
#include <cstdio>


const Matrix3 Matrix3::Identity = Matrix3();



Matrix3::Matrix3()
{
	data[0] = 1.0f;
	data[1] = 0.0f;
	data[2] = 0.0f;
	data[3] = 0.0f;
	data[4] = 1.0f;
	data[5] = 0.0f;
	data[6] = 0.0f;
	data[7] = 0.0f;
	data[8] = 1.0f;
}


Matrix3::Matrix3(const Matrix3& other)
{
	memcpy(data, other.data, 9*4);
}


Matrix3::Matrix3(	float m00, float m10, float m20,
					float m01, float m11, float m21,
					float m02, float m12, float m22
)
{
	data[0] = m00;
	data[1] = m10;
	data[2] = m20;
	data[3] = m01;
	data[4] = m11;
	data[5] = m21;
	data[6] = m02;
	data[7] = m12;
	data[8] = m22;
}


Matrix3 Matrix3::fromEulerYZX(float x, float y, float z)
{
	float sx = sinf(x);
	float cx = cosf(x);
	float sy = sinf(y);
	float cy = cosf(y);
	float sz = sinf(z);
	float cz = cosf(z);

	return Matrix3 (
			cy*cz,				sz,				-sy*cz,
			-cy*sz*cx + sy*sx,	cz*cx,			sy*sz*cx + cy*sx,
			cy*sz*sx + sy*cx,	-cz*cx,			-sy*sz*sx + cy*cx
	);
}


Matrix3& Matrix3::operator+=(const Matrix3& other)
{
	data[0] += other.data[0];
	data[1] += other.data[1];
	data[2] += other.data[2];
	data[3] += other.data[3];
	data[4] += other.data[4];
	data[5] += other.data[5];
	data[6] += other.data[6];
	data[7] += other.data[7];
	data[8] += other.data[8];
	return *this;
}


Matrix3& Matrix3::operator-=(const Matrix3& other)
{
	data[0] -= other.data[0];
	data[1] -= other.data[1];
	data[2] -= other.data[2];
	data[3] -= other.data[3];
	data[4] -= other.data[4];
	data[5] -= other.data[5];
	data[6] -= other.data[6];
	data[7] -= other.data[7];
	data[8] -= other.data[8];
	return *this;
}


Matrix3& Matrix3::operator*=(float rhv)
{
	data[0] *= rhv;
	data[1] *= rhv;
	data[2] *= rhv;
	data[3] *= rhv;
	data[4] *= rhv;
	data[5] *= rhv;
	data[6] *= rhv;
	data[7] *= rhv;
	data[8] *= rhv;
	return *this;
}


const Vector3 Matrix3::row(int i) const
{
	return Vector3(data[0+i], data[3+i], data[6+i]);
}


const Vector3 Matrix3::column(int i) const
{
	return Vector3(data[0+i*3], data[1+i*3], data[2+i*3]);
}


Matrix3& Matrix3::operator*=(const Matrix3& rhv)
{
	Vector3 r0 = row(0);
	Vector3 r1 = row(1);
	Vector3 r2 = row(2);
	Vector3 c0 = rhv.column(0);
	Vector3 c1 = rhv.column(1);
	Vector3 c2 = rhv.column(2);

	data[0] = r0.dot(c0);
	data[1] = r1.dot(c0);
	data[2] = r2.dot(c0);
	data[3] = r0.dot(c1);
	data[4] = r1.dot(c1);
	data[5] = r2.dot(c1);
	data[6] = r0.dot(c2);
	data[7] = r1.dot(c2);
	data[8] = r2.dot(c2);
	return *this;
}


const Vector3 Matrix3::operator*(const Vector3& rhv) const
{
	float x = rhv.getX();
	float y = rhv.getY();
	float z = rhv.getZ();

	return Vector3 (
			data[0]*x + data[3]*y + data[6]*z,
			data[1]*x + data[4]*y + data[7]*z,
			data[2]*x + data[5]*y + data[8]*z
	);
}


void Matrix3::transpose()
{
	float odata[9];
	memcpy(odata, data, sizeof(data));
	data[0] = odata[0];
	data[1] = odata[3];
	data[2] = odata[6];
	data[3] = odata[1];
	data[4] = odata[4];
	data[5] = odata[7];
	data[6] = odata[2];
	data[7] = odata[5];
	data[8] = odata[8];
}


void Matrix3::printDebug()
{
	printf("(\t%f\t%f\t%f\n\t%f\t%f\t%f\n\t%f\t%f\t%f\n)\n", data[0], data[1], data[2],
			data[3], data[4], data[5], data[6], data[7], data[8]);
}
