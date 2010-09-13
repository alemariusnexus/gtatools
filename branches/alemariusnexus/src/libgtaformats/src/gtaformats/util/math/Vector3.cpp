/*
 * Vector3.cpp
 *
 *  Created on: 06.08.2010
 *      Author: alemariusnexus
 */

#include "Vector3.h"
#include <cmath>
#include <cstdio>


/*const Vector3 Vector3::ZERO = Vector3();
const Vector3 Vector3::UNIT_X = Vector3(1.0f, 0.0f, 0.0f);
const Vector3 Vector3::UNIT_Y = Vector3(0.0f, 1.0f, 0.0f);
const Vector3 Vector3::UNIT_Z = Vector3(0.0f, 0.0f, 1.0f);
const Vector3 Vector3::NEGATIVE_UNIT_X = Vector3(-1.0f, 0.0f, 0.0f);
const Vector3 Vector3::NEGATIVE_UNIT_Y = Vector3(0.0f, -1.0f, 0.0f);
const Vector3 Vector3::NEGATIVE_UNIT_Z = Vector3(0.0f, 0.0f, -1.0f);*/



Vector3& Vector3::operator=(const Vector3& rhv)
{
	if (this != &rhv) {
		memcpy(data, rhv.data, sizeof(data));
	}

	return *this;
}


Vector3& Vector3::operator+=(const Vector3& rhv)
{
	data[0] += rhv.data[0];
	data[1] += rhv.data[1];
	data[2] += rhv.data[2];
	return *this;
}


Vector3& Vector3::operator-=(const Vector3& rhv)
{
	data[0] -= rhv.data[0];
	data[1] -= rhv.data[1];
	data[2] -= rhv.data[2];
	return *this;
}


Vector3& Vector3::operator*=(float rhv)
{
	data[0] *= rhv;
	data[1] *= rhv;
	data[2] *= rhv;
	return *this;
}


const Vector3 Vector3::operator-() const
{
	return Vector3(-data[0], -data[1], -data[2]);
}


float Vector3::dot(const Vector3& rhv) const
{
	return data[0]*rhv.data[0] + data[1]*rhv.data[1] + data[2]*rhv.data[2];
}


const Vector3 Vector3::cross(const Vector3& rhv) const
{
	return Vector3 (
			data[1]*rhv[2] - data[2]*rhv[1],
			data[2]*rhv[0] - data[0]*rhv[2],
			data[0]*rhv[1] - data[1]*rhv[0]
	);
}


float Vector3::length() const
{
	return sqrtf(this->dot(*this));
}


float Vector3::normalize()
{
	float len = length();

	data[0] /= len;
	data[1] /= len;
	data[2] /= len;

	return len;
}


float Vector3::angle(const Vector3& other) const
{
	float d = dot(other);

	if (d > 1.0f)
		d = 1.0f;
	else if (d < -1.0f)
		d = -1.0f;

	return acosf(d);
}
