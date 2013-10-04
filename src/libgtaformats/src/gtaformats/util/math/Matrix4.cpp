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

#include "Matrix4.h"
#include "Matrix3.h"
#include <cstring>
#include <cmath>
#include <cstdlib>
#include <cstdio>

#define GET_RC(r,c) data[(c)*4+(r)]

#ifndef M_PI_2
# define M_PI_2		1.57079632679489661923
#endif


const Matrix4 Matrix4::Identity = Matrix4();



Matrix4::Matrix4()
{
	data[0] = 1.0f;
	data[1] = 0.0f;
	data[2] = 0.0f;
	data[3] = 0.0f;
	data[4] = 0.0f;
	data[5] = 1.0f;
	data[6] = 0.0f;
	data[7] = 0.0f;
	data[8] = 0.0f;
	data[9] = 0.0f;
	data[10] = 1.0f;
	data[11] = 0.0f;
	data[12] = 0.0f;
	data[13] = 0.0f;
	data[14] = 0.0f;
	data[15] = 1.0f;
}


Matrix4::Matrix4(const Matrix3& other)
{
	const float* odata = other.toArray();

	data[0] = odata[0];
	data[1] = odata[1];
	data[2] = odata[2];
	data[3] = 0.0f;
	data[4] = odata[3];
	data[5] = odata[4];
	data[6] = odata[5];
	data[7] = 0.0f;
	data[8] = odata[6];
	data[9] = odata[7];
	data[10] = odata[8];
	data[11] = 0.0f;
	data[12] = 0.0f;
	data[13] = 0.0f;
	data[14] = 0.0f;
	data[15] = 1.0f;
}


Matrix4::Matrix4(	float m00, float m10, float m20, float m30,
					float m01, float m11, float m21, float m31,
					float m02, float m12, float m22, float m32,
					float m03, float m13, float m23, float m33
)
{
	data[0] = m00;
	data[1] = m10;
	data[2] = m20;
	data[3] = m30;
	data[4] = m01;
	data[5] = m11;
	data[6] = m21;
	data[7] = m31;
	data[8] = m02;
	data[9] = m12;
	data[10] = m22;
	data[11] = m32;
	data[12] = m03;
	data[13] = m13;
	data[14] = m23;
	data[15] = m33;
}


Matrix4::Matrix4(const Matrix4& other)
{
	memcpy(data, other.data, 16*4);
}


Matrix4& Matrix4::operator+=(const Matrix4& other)
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
	data[9] += other.data[9];
	data[10] += other.data[10];
	data[11] += other.data[11];
	data[12] += other.data[12];
	data[13] += other.data[13];
	data[14] += other.data[14];
	data[15] += other.data[15];
	return *this;
}


Matrix4& Matrix4::operator-=(const Matrix4& other)
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
	data[9] -= other.data[9];
	data[10] -= other.data[10];
	data[11] -= other.data[11];
	data[12] -= other.data[12];
	data[13] -= other.data[13];
	data[14] -= other.data[14];
	data[15] -= other.data[15];
	return *this;
}


Matrix4& Matrix4::operator*=(float rhv)
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
	data[9] *= rhv;
	data[10] *= rhv;
	data[11] *= rhv;
	data[12] *= rhv;
	data[13] *= rhv;
	data[14] *= rhv;
	data[15] *= rhv;
	return *this;
}


const Vector4 Matrix4::row(int i) const
{
	return Vector4(data[0+i], data[4+i], data[8+i], data[12+i]);
}


const Vector4 Matrix4::column(int i) const
{
	return Vector4(data[0+i*4], data[1+i*4], data[2+i*4], data[3+i*4]);
}


Matrix4& Matrix4::operator*=(const Matrix4& rhv)
{
	float newData[16];

	newData[0] = data[0]*rhv.data[0] + data[4]*rhv.data[1] + data[8]*rhv.data[2] + data[12]*rhv.data[3];
	newData[1] = data[1]*rhv.data[0] + data[5]*rhv.data[1] + data[9]*rhv.data[2] + data[13]*rhv.data[3];
	newData[2] = data[2]*rhv.data[0] + data[6]*rhv.data[1] + data[10]*rhv.data[2] + data[14]*rhv.data[3];
	newData[3] = data[3]*rhv.data[0] + data[7]*rhv.data[1] + data[11]*rhv.data[2] + data[15]*rhv.data[3];

	newData[4] = data[0]*rhv.data[4] + data[4]*rhv.data[5] + data[8]*rhv.data[6] + data[12]*rhv.data[7];
	newData[5] = data[1]*rhv.data[4] + data[5]*rhv.data[5] + data[9]*rhv.data[6] + data[13]*rhv.data[7];
	newData[6] = data[2]*rhv.data[4] + data[6]*rhv.data[5] + data[10]*rhv.data[6] + data[14]*rhv.data[7];
	newData[7] = data[3]*rhv.data[4] + data[7]*rhv.data[5] + data[11]*rhv.data[6] + data[15]*rhv.data[7];

	newData[8] = data[0]*rhv.data[8] + data[4]*rhv.data[9] + data[8]*rhv.data[10] + data[12]*rhv.data[11];
	newData[9] = data[1]*rhv.data[8] + data[5]*rhv.data[9] + data[9]*rhv.data[10] + data[13]*rhv.data[11];
	newData[10] = data[2]*rhv.data[8] + data[6]*rhv.data[9] + data[10]*rhv.data[10] + data[14]*rhv.data[11];
	newData[11] = data[3]*rhv.data[8] + data[7]*rhv.data[9] + data[11]*rhv.data[10] + data[15]*rhv.data[11];

	newData[12] = data[0]*rhv.data[12] + data[4]*rhv.data[13] + data[8]*rhv.data[14] + data[12]*rhv.data[15];
	newData[13] = data[1]*rhv.data[12] + data[5]*rhv.data[13] + data[9]*rhv.data[14] + data[13]*rhv.data[15];
	newData[14] = data[2]*rhv.data[12] + data[6]*rhv.data[13] + data[10]*rhv.data[14] + data[14]*rhv.data[15];
	newData[15] = data[3]*rhv.data[12] + data[7]*rhv.data[13] + data[11]*rhv.data[14] + data[15]*rhv.data[15];

	memcpy(data, newData, 16*sizeof(float));

	return *this;
}


const Vector4 Matrix4::operator*(const Vector4& rhv) const
{
	float x = rhv.getX();
	float y = rhv.getY();
	float z = rhv.getZ();
	float w = rhv.getW();

	return Vector4 (
			data[0]*x + data[4]*y + data[8]*z + data[12]*w,
			data[1]*x + data[5]*y + data[9]*z + data[13]*w,
			data[2]*x + data[6]*y + data[10]*z + data[14]*w,
			data[3]*x + data[7]*y + data[11]*z + data[15]*w
	);
}


void Matrix4::setTranslation(float x, float y, float z)
{
	data[12] = x;
	data[13] = y;
	data[14] = z;
}


Matrix4 Matrix4::translation(float x, float y, float z)
{
	return Matrix4 (
			1.0f,	0.0f,	0.0f,	0.0f,
			0.0f,	1.0f,	0.0f,	0.0f,
			0.0f,	0.0f,	1.0f,	0.0f,
			x,		y,		z,		1.0f
	);
}


Matrix4 Matrix4::rotation(float angle, const Vector3& vec)
{
	Vector3 vecCpy = Vector3(vec);
	vecCpy.normalize();
	float x = vecCpy.getX();
	float y = vecCpy.getY();
	float z = vecCpy.getZ();
	float s = sinf(angle);
	float c = cosf(angle);

	return Matrix4 (
			x*x*(1-c)+c,	x*y*(1-c)+z*s,	x*z*(1-c)-y*s,	0,
			x*y*(1-c)-z*s,	y*y*(1-c)+c,	y*z*(1-c)+x*s,	0,
			x*z*(1-c)+y*s,	y*z*(1-c)-x*s,	z*z*(1-c)+c,	0,
			0,				0,				0,				1
	);
}


Matrix4 Matrix4::rotationX(float a)
{
	float s = sinf(a);
	float c = cosf(a);
	return Matrix4 (
			1.0f,		0.0f,		0.0f,		0.0f,
			0.0f,		c,			s,			0.0f,
			0.0f,		-s,			c,			0.0f,
			0.0f,		0.0f,		0.0f,		1.0f
	);
}


Matrix4 Matrix4::rotationY(float a)
{
	float s = sinf(a);
	float c = cosf(a);
	return Matrix4 (
			c,			0.0f,		-s,			0.0f,
			0.0f,		1.0f,		0.0f,		0.0f,
			s,			0.0f,		c,			0.0f,
			0.0f,		0.0f,		0.0f,		1.0f
	);
}


Matrix4 Matrix4::rotationZ(float a)
{
	float s = sinf(a);
	float c = cosf(a);
	return Matrix4 (
			c,			s,			0.0f,		0.0f,
			-s,			c,			0.0f,		0.0f,
			0.0f,		0.0f,		1.0f,		0.0f,
			0.0f,		0.0f,		0.0f,		1.0f
	);
}


Matrix4 Matrix4::scale(const Vector3& s)
{
	return Matrix4 (
			s.getX(),	0.0f,		0.0f,		0.0f,
			0.0f,		s.getY(),	0.0f,		0.0f,
			0.0f,		0.0f,		s.getZ(),	0.0f,
			0.0f,		0.0f,		0.0f,		1.0f
	);
}


Matrix4 Matrix4::lookAt(const Vector3& target, const Vector3& up)
{
	Vector3 f = target;
	f.normalize();
	Vector3 upn = up;
	upn.normalize();
	Vector3 s = f.cross(upn);
	Vector3 u = s.cross(f);

	return Matrix4 (
			s.getX(), u.getX(), -f.getX(), 0,
			s.getY(), u.getY(), -f.getY(), 0,
			s.getZ(), u.getZ(), -f.getZ(), 0,
			0, 0, 0, 1
	);
}


Matrix4 Matrix4::fromQuaternion(const Quaternion& q)
{
	float w = q.getW();
	float x = q.getX();
	float y = q.getY();
	float z = q.getZ();

	float x2 = x*x;
	float y2 = y*y;
	float z2 = z*z;
	float xy = x*y;
	float zw = z*w;
	float xz = x*z;
	float yw = y*w;
	float yz = y*z;
	float xw = x*w;

	return Matrix4 (
			1.0f - 2.0f * (y2+z2),	2.0f * (xy-zw),			2.0f * (xz+yw),			0.0f,
			2.0f * (xy+zw),			1.0f - 2.0f * (x2+z2),	2.0f * (yz-xw),			0.0f,
			2.0f * (xz-yw),			2.0f * (yz+xw),			1.0f - 2.0f * (x2+y2),	0.0f,
			0.0f,					0.0f,					0.0f,					1.0f
	);
}


Matrix4 Matrix4::fromQuaternionVector(const Vector3& pos, const Quaternion& rot)
{
	float w = rot.getW();
	float x = rot.getX();
	float y = rot.getY();
	float z = rot.getZ();

	float x2 = x*x;
	float y2 = y*y;
	float z2 = z*z;
	float xy = x*y;
	float zw = z*w;
	float xz = x*z;
	float yw = y*w;
	float yz = y*z;
	float xw = x*w;

	return Matrix4 (
			1.0f - 2.0f * (y2+z2),	2.0f * (xy-zw),			2.0f * (xz+yw),			0.0f,
			2.0f * (xy+zw),			1.0f - 2.0f * (x2+z2),	2.0f * (yz-xw),			0.0f,
			2.0f * (xz-yw),			2.0f * (yz+xw),			1.0f - 2.0f * (x2+y2),	0.0f,
			pos.getX(),				pos.getY(),				pos.getZ(),				1.0f
	);
}


void Matrix4::transpose()
{
	float odata[16];
	memcpy(odata, data, 16*sizeof(float));

	data[0] = odata[0];
	data[1] = odata[4];
	data[2] = odata[8];
	data[3] = odata[12];
	data[4] = odata[1];
	data[5] = odata[5];
	data[6] = odata[9];
	data[7] = odata[13];
	data[8] = odata[2];
	data[9] = odata[6];
	data[10] = odata[10];
	data[11] = odata[14];
	data[12] = odata[3];
	data[13] = odata[7];
	data[14] = odata[11];
	data[15] = odata[15];
}


float Matrix4::determinant() const
{
	return	  GET_RC(0,3) * GET_RC(1,2) * GET_RC(2,1) * GET_RC(3,0)
			- GET_RC(0,2) * GET_RC(1,3) * GET_RC(2,1) * GET_RC(3,0)
			- GET_RC(0,3) * GET_RC(1,1) * GET_RC(2,2) * GET_RC(3,0)
			+ GET_RC(0,1) * GET_RC(1,3) * GET_RC(2,2) * GET_RC(3,0)
			+ GET_RC(0,2) * GET_RC(1,1) * GET_RC(2,3) * GET_RC(3,0)
			- GET_RC(0,1) * GET_RC(1,2) * GET_RC(2,3) * GET_RC(3,0)
			- GET_RC(0,3) * GET_RC(1,2) * GET_RC(2,0) * GET_RC(3,1)
			+ GET_RC(0,2) * GET_RC(1,3) * GET_RC(2,0) * GET_RC(3,1)
			+ GET_RC(0,3) * GET_RC(1,0) * GET_RC(2,2) * GET_RC(3,1)
			- GET_RC(0,0) * GET_RC(1,3) * GET_RC(2,2) * GET_RC(3,1)
			- GET_RC(0,2) * GET_RC(1,0) * GET_RC(2,3) * GET_RC(3,1)
			+ GET_RC(0,0) * GET_RC(1,2) * GET_RC(2,3) * GET_RC(3,1)
			+ GET_RC(0,3) * GET_RC(1,1) * GET_RC(2,0) * GET_RC(3,2)
			- GET_RC(0,1) * GET_RC(1,3) * GET_RC(2,0) * GET_RC(3,2)
			- GET_RC(0,3) * GET_RC(1,0) * GET_RC(2,1) * GET_RC(3,2)
			+ GET_RC(0,0) * GET_RC(1,3) * GET_RC(2,1) * GET_RC(3,2)
			+ GET_RC(0,1) * GET_RC(1,0) * GET_RC(2,3) * GET_RC(3,2)
			- GET_RC(0,0) * GET_RC(1,1) * GET_RC(2,3) * GET_RC(3,2)
			- GET_RC(0,2) * GET_RC(1,1) * GET_RC(2,0) * GET_RC(3,3)
			+ GET_RC(0,1) * GET_RC(1,2) * GET_RC(2,0) * GET_RC(3,3)
			+ GET_RC(0,2) * GET_RC(1,0) * GET_RC(2,1) * GET_RC(3,3)
			- GET_RC(0,0) * GET_RC(1,2) * GET_RC(2,1) * GET_RC(3,3)
			- GET_RC(0,1) * GET_RC(1,0) * GET_RC(2,2) * GET_RC(3,3)
			+ GET_RC(0,0) * GET_RC(1,1) * GET_RC(2,2) * GET_RC(3,3);
}


void Matrix4::invert()
{
	float data[16];
	memcpy(data, this->data, 16*sizeof(float));

	float det = determinant();

	this->GET_RC(0,0) =		  GET_RC(1,2)*GET_RC(2,3)*GET_RC(3,1) - GET_RC(1,3)*GET_RC(2,2)*GET_RC(3,1)
							+ GET_RC(1,3)*GET_RC(2,1)*GET_RC(3,2) - GET_RC(1,1)*GET_RC(2,3)*GET_RC(3,2)
							- GET_RC(1,2)*GET_RC(2,1)*GET_RC(3,3) + GET_RC(1,1)*GET_RC(2,2)*GET_RC(3,3);
	this->GET_RC(0,1) =		  GET_RC(0,3)*GET_RC(2,2)*GET_RC(3,1) - GET_RC(0,2)*GET_RC(2,3)*GET_RC(3,1)
							- GET_RC(0,3)*GET_RC(2,1)*GET_RC(3,2) + GET_RC(0,1)*GET_RC(2,3)*GET_RC(3,2)
							+ GET_RC(0,2)*GET_RC(2,1)*GET_RC(3,3) - GET_RC(0,1)*GET_RC(2,2)*GET_RC(3,3);
	this->GET_RC(0,2) =		  GET_RC(0,2)*GET_RC(1,3)*GET_RC(3,1) - GET_RC(0,3)*GET_RC(1,2)*GET_RC(3,1)
							+ GET_RC(0,3)*GET_RC(1,1)*GET_RC(3,2) - GET_RC(0,1)*GET_RC(1,3)*GET_RC(3,2)
							- GET_RC(0,2)*GET_RC(1,1)*GET_RC(3,3) + GET_RC(0,1)*GET_RC(1,2)*GET_RC(3,3);
	this->GET_RC(0,3) =		  GET_RC(0,3)*GET_RC(1,2)*GET_RC(2,1) - GET_RC(0,2)*GET_RC(1,3)*GET_RC(2,1)
							- GET_RC(0,3)*GET_RC(1,1)*GET_RC(2,2) + GET_RC(0,1)*GET_RC(1,3)*GET_RC(2,2)
							+ GET_RC(0,2)*GET_RC(1,1)*GET_RC(2,3) - GET_RC(0,1)*GET_RC(1,2)*GET_RC(2,3);
	this->GET_RC(1,0) =		  GET_RC(1,3)*GET_RC(2,2)*GET_RC(3,0) - GET_RC(1,2)*GET_RC(2,3)*GET_RC(3,0)
							- GET_RC(1,3)*GET_RC(2,0)*GET_RC(3,2) + GET_RC(1,0)*GET_RC(2,3)*GET_RC(3,2)
							+ GET_RC(1,2)*GET_RC(2,0)*GET_RC(3,3) - GET_RC(1,0)*GET_RC(2,2)*GET_RC(3,3);
	this->GET_RC(1,1) =		  GET_RC(0,2)*GET_RC(2,3)*GET_RC(3,0) - GET_RC(0,3)*GET_RC(2,2)*GET_RC(3,0)
							+ GET_RC(0,3)*GET_RC(2,0)*GET_RC(3,2) - GET_RC(0,0)*GET_RC(2,3)*GET_RC(3,2)
							- GET_RC(0,2)*GET_RC(2,0)*GET_RC(3,3) + GET_RC(0,0)*GET_RC(2,2)*GET_RC(3,3);
	this->GET_RC(1,2) =		  GET_RC(0,3)*GET_RC(1,2)*GET_RC(3,0) - GET_RC(0,2)*GET_RC(1,3)*GET_RC(3,0)
							- GET_RC(0,3)*GET_RC(1,0)*GET_RC(3,2) + GET_RC(0,0)*GET_RC(1,3)*GET_RC(3,2)
							+ GET_RC(0,2)*GET_RC(1,0)*GET_RC(3,3) - GET_RC(0,0)*GET_RC(1,2)*GET_RC(3,3);
	this->GET_RC(1,3) =		  GET_RC(0,2)*GET_RC(1,3)*GET_RC(2,0) - GET_RC(0,3)*GET_RC(1,2)*GET_RC(2,0)
							+ GET_RC(0,3)*GET_RC(1,0)*GET_RC(2,2) - GET_RC(0,0)*GET_RC(1,3)*GET_RC(2,2)
							- GET_RC(0,2)*GET_RC(1,0)*GET_RC(2,3) + GET_RC(0,0)*GET_RC(1,2)*GET_RC(2,3);
	this->GET_RC(2,0) =		  GET_RC(1,1)*GET_RC(2,3)*GET_RC(3,0) - GET_RC(1,3)*GET_RC(2,1)*GET_RC(3,0)
							+ GET_RC(1,3)*GET_RC(2,0)*GET_RC(3,1) - GET_RC(1,0)*GET_RC(2,3)*GET_RC(3,1)
							- GET_RC(1,1)*GET_RC(2,0)*GET_RC(3,3) + GET_RC(1,0)*GET_RC(2,1)*GET_RC(3,3);
	this->GET_RC(2,1) =		  GET_RC(0,3)*GET_RC(2,1)*GET_RC(3,0) - GET_RC(0,1)*GET_RC(2,3)*GET_RC(3,0)
							- GET_RC(0,3)*GET_RC(2,0)*GET_RC(3,1) + GET_RC(0,0)*GET_RC(2,3)*GET_RC(3,1)
							+ GET_RC(0,1)*GET_RC(2,0)*GET_RC(3,3) - GET_RC(0,0)*GET_RC(2,1)*GET_RC(3,3);
	this->GET_RC(2,2) =		  GET_RC(0,1)*GET_RC(1,3)*GET_RC(3,0) - GET_RC(0,3)*GET_RC(1,1)*GET_RC(3,0)
							+ GET_RC(0,3)*GET_RC(1,0)*GET_RC(3,1) - GET_RC(0,0)*GET_RC(1,3)*GET_RC(3,1)
							- GET_RC(0,1)*GET_RC(1,0)*GET_RC(3,3) + GET_RC(0,0)*GET_RC(1,1)*GET_RC(3,3);
	this->GET_RC(2,3) =		  GET_RC(0,3)*GET_RC(1,1)*GET_RC(2,0) - GET_RC(0,1)*GET_RC(1,3)*GET_RC(2,0)
							- GET_RC(0,3)*GET_RC(1,0)*GET_RC(2,1) + GET_RC(0,0)*GET_RC(1,3)*GET_RC(2,1)
							+ GET_RC(0,1)*GET_RC(1,0)*GET_RC(2,3) - GET_RC(0,0)*GET_RC(1,1)*GET_RC(2,3);
	this->GET_RC(3,0) =		  GET_RC(1,2)*GET_RC(2,1)*GET_RC(3,0) - GET_RC(1,1)*GET_RC(2,2)*GET_RC(3,0)
							- GET_RC(1,2)*GET_RC(2,0)*GET_RC(3,1) + GET_RC(1,0)*GET_RC(2,2)*GET_RC(3,1)
							+ GET_RC(1,1)*GET_RC(2,0)*GET_RC(3,2) - GET_RC(1,0)*GET_RC(2,1)*GET_RC(3,2);
	this->GET_RC(3,1) =		  GET_RC(0,1)*GET_RC(2,2)*GET_RC(3,0) - GET_RC(0,2)*GET_RC(2,1)*GET_RC(3,0)
							+ GET_RC(0,2)*GET_RC(2,0)*GET_RC(3,1) - GET_RC(0,0)*GET_RC(2,2)*GET_RC(3,1)
							- GET_RC(0,1)*GET_RC(2,0)*GET_RC(3,2) + GET_RC(0,0)*GET_RC(2,1)*GET_RC(3,2);
	this->GET_RC(3,2) =		  GET_RC(0,2)*GET_RC(1,1)*GET_RC(3,0) - GET_RC(0,1)*GET_RC(1,2)*GET_RC(3,0)
							- GET_RC(0,2)*GET_RC(1,0)*GET_RC(3,1) + GET_RC(0,0)*GET_RC(1,2)*GET_RC(3,1)
							+ GET_RC(0,1)*GET_RC(1,0)*GET_RC(3,2) - GET_RC(0,0)*GET_RC(1,1)*GET_RC(3,2);
	this->GET_RC(3,3) =		  GET_RC(0,1)*GET_RC(1,2)*GET_RC(2,0) - GET_RC(0,2)*GET_RC(1,1)*GET_RC(2,0)
							+ GET_RC(0,2)*GET_RC(1,0)*GET_RC(2,1) - GET_RC(0,0)*GET_RC(1,2)*GET_RC(2,1)
							- GET_RC(0,1)*GET_RC(1,0)*GET_RC(2,2) + GET_RC(0,0)*GET_RC(1,1)*GET_RC(2,2);

	*this *= (1.0f / det);
}


void Matrix4::toEuler(float& x, float& y, float& z) // b, h, a
{
	if (GET_RC(1, 0) > 0.998f) {
		y = atan2f(GET_RC(0, 2), GET_RC(2, 2));
		z = M_PI_2;
		x = 0.0f;
		return;
	} else if (GET_RC(1, 0) < -0.998f) {
		y = atan2f(GET_RC(0, 2), GET_RC(2, 2));
		z = -M_PI_2;
		x = 0.0f;
		return;
	}

	y = atan2f(-GET_RC(2, 0), GET_RC(0, 0)); // -20, 00
	x = atan2f(-GET_RC(1, 2), GET_RC(1, 1)); // -12 11
	z = asinf(GET_RC(1, 0));
}


Matrix4 Matrix4::frustum(float l, float r, float b, float t, float n, float f)
{
	return Matrix4 (
			2*n / (r-l),		0,					0,					0,
			0,					2*n / (t-b),		0,					0,
			(r+l) / (r-l),		(t+b) / (t-b),		(-(f+n)) / (f-n),	-1,
			0,					0,					(-2*f*n) / (f-n),	0
	);
}


Matrix4 Matrix4::perspective(float fovy, float aspect, float n, float f)
{
	float fc = 1.0f / tanf(fovy / 2.0f);

	return Matrix4 (
			fc / aspect,		0,					0,					0,
			0,					fc,					0,					0,
			0,					0,					(f+n) / (n-f),		-1,
			0,					0,					(2*f*n) / (n-f),	0
	);
}


Matrix4 Matrix4::ortho(float l, float r, float b, float t, float n, float f)
{
	return Matrix4 (
			2.0f / (r-l),		0,					0,					0,
			0,					2.0f / (t-b),		0,					0,
			0,					0,					-2.0f / (f-n),		0,
			-((r+l) / (r-l)),	-((t+b) / (t-b)),	-((f+n) / (f-n)),	1
	);
}
