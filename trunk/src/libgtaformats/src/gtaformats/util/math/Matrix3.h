/*
	Copyright 2010-2011 David "Alemarius Nexus" Lerch

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

#ifndef MATRIX3_H_
#define MATRIX3_H_

#include "../../config.h"
#include "Vector3.h"


class Matrix3
{
public:
	Matrix3();
	Matrix3(const Matrix3& other);
	Matrix3(float* data) { memcpy(this->data, data, 9*4); }
	Matrix3(float m00, float m10, float m20,
			float m01, float m11, float m21,
			float m02, float m12, float m22);
	const float* toArray() const { return data; }
	float* toArray() { return data; }
	Matrix3& operator+=(const Matrix3& other);
	const Matrix3 operator+(const Matrix3& rhv) const { return Matrix3(*this) += rhv; }
	Matrix3& operator-=(const Matrix3& other);
	const Matrix3 operator-(const Matrix3& rhv) const { return Matrix3(*this) -= rhv; }
	Matrix3& operator*=(float rhv);
	const Matrix3 operator*(float rhv) const { return Matrix3(*this) *= rhv; }
	Matrix3& operator*=(const Matrix3& rhv);
	const Matrix3 operator*(const Matrix3& rhv) const { return Matrix3(*this) *= rhv; }
	const Vector3 row(int i) const;
	const Vector3 column(int i) const;
	void transpose();
	void printDebug();

private:
	float data[9];
};


#endif /* MATRIX3_H_ */
