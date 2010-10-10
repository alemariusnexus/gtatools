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

#ifndef MATRIX4_H_
#define MATRIX4_H_

#include "../../gf_config.h"
#include "Vector3.h"
#include "Vector4.h"

class Matrix3;


class Matrix4
{
public:
	Matrix4();
	Matrix4(const Matrix4& other);
	Matrix4(float m00, float m10, float m20, float m30,
			float m01, float m11, float m21, float m31,
			float m02, float m12, float m22, float m32,
			float m03, float m13, float m23, float m33);
	Matrix4(float* data) { memcpy(this->data, data, 16*4); }
	Matrix4(const Matrix3& other);
	const float* toArray() const { return data; }
	float* toArray() { return data; }
	Matrix4& operator+=(const Matrix4& other);
	const Matrix4 operator+(const Matrix4& rhv) const { return Matrix4(*this) += rhv; }
	Matrix4& operator-=(const Matrix4& other);
	const Matrix4 operator-(const Matrix4& rhv) const { return Matrix4(*this) -= rhv; }
	Matrix4& operator*=(float rhv);
	const Matrix4 operator*(float rhv) const { return Matrix4(*this) *= rhv; }
	Matrix4& operator*=(const Matrix4& rhv);
	const Matrix4 operator*(const Matrix4& rhv) const { return Matrix4(*this) *= rhv; }
	const Vector4 operator*(const Vector4& rhv) const;
	const Vector4 row(int i) const;
	const Vector4 column(int i) const;
	void translate(float x, float y, float z) { *this *= translation(x, y, z); }
	void translate(const Vector3& vec) { *this *= translation(vec); }
	void rotate(float angle, const Vector3& vec) { *this *= rotation(angle, vec); }
	void rotate(float angle, float x, float y, float z) { *this *= rotation(angle, x, y, z); }
	void rotateX(float angle) { *this *= rotationX(angle); }
	void rotateY(float angle) { *this *= rotationY(angle); }
	void rotateZ(float angle) { *this *= rotationZ(angle); }
	void setTranslation(float x, float y, float z);
	void setTranslation(const Vector3& vec) { setTranslation(vec.getX(), vec.getY(), vec.getZ()); }

public:
	static Matrix4 translation(float x, float y, float z);
	static Matrix4 translation(const Vector3& vec) { return translation(vec.getX(), vec.getY(), vec.getZ()); }
	static Matrix4 rotation(float angle, const Vector3& vec);
	static Matrix4 rotation(float a, float x, float y, float z) { return rotation(a, Vector3(x, y, z)); }
	static Matrix4 rotationX(float a);
	static Matrix4 rotationY(float a);
	static Matrix4 rotationZ(float a);
	static Matrix4 lookAt(const Vector3& target, const Vector3& up);
	static Matrix4 fromQuaternion(float x, float y, float z, float w);

private:
	float data[16];
};

#endif /* MATRIX4_H_ */
