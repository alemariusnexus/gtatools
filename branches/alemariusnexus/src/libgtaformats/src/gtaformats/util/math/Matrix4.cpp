/*
 * Matrix3.cpp
 *
 *  Created on: 06.08.2010
 *      Author: alemariusnexus
 */

#include "Matrix4.h"
#include "Matrix3.h"
#include <cstring>
#include <cmath>


/*const Matrix4 Matrix4::IDENTITY = Matrix4 (
		1.0f,	0.0f,	0.0f,	0.0f,
		0.0f,	1.0f,	0.0f,	0.0f,
		0.0f,	0.0f,	1.0f,	0.0f,
		0.0f,	0.0f,	0.0f,	1.0f
);*/



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
	Vector4 r0 = row(0);
	Vector4 r1 = row(1);
	Vector4 r2 = row(2);
	Vector4 r3 = row(3);
	Vector4 c0 = rhv.column(0);
	Vector4 c1 = rhv.column(1);
	Vector4 c2 = rhv.column(2);
	Vector4 c3 = rhv.column(3);

	data[0] = r0.dot(c0);
	data[1] = r1.dot(c0);
	data[2] = r2.dot(c0);
	data[3] = r3.dot(c0);
	data[4] = r0.dot(c1);
	data[5] = r1.dot(c1);
	data[6] = r2.dot(c1);
	data[7] = r3.dot(c1);
	data[8] = r0.dot(c2);
	data[9] = r1.dot(c2);
	data[10] = r2.dot(c2);
	data[11] = r3.dot(c2);
	data[12] = r0.dot(c3);
	data[13] = r1.dot(c3);
	data[14] = r2.dot(c3);
	data[15] = r3.dot(c3);
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

	/*Vector3 cross = up.cross(target);
	cross.normalize();

	float tx = target.getX();
	float ty = target.getY();
	float tz = target.getZ();
	float ux = up.getX();
	float uy = up.getY();
	float uz = up.getZ();
	float cx = cross.getX();
	float cy = cross.getY();
	float cz = cross.getZ();

	return Matrix4 (
			cx,	ux, -tx,	0,
			cy,	uy, -ty, 0,
			cz, uz, -tz, 0,
			0,	0,	0,	1
	);*/
}
