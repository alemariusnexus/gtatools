/*
 * Matrix3.h
 *
 *  Created on: 06.08.2010
 *      Author: alemariusnexus
 */

#ifndef MATRIX3_H_
#define MATRIX3_H_

#include <gf_config.h>
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

private:
	float data[9];
};


#endif /* MATRIX3_H_ */