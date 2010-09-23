/*
 * Vector3.h
 *
 *  Created on: 06.08.2010
 *      Author: alemariusnexus
 */

#ifndef VECTOR4_H_
#define VECTOR4_H_

#include "../../gf_config.h"
#include <cstring>

// TODO Replace 'int' with 'int32_t' (gave strange linker errors)

class Vector3;

typedef float v4sf __attribute__((vector_size(16)));
typedef int v4i32 __attribute__((vector_size(16)));

union Vec4SF
{
	v4sf v;
	float f[4];
};

union Vec4I32
{
	v4i32 v;
	int i[4];
};


class Vector4
{
private:
	friend class Vector3;

public:
	Vector4() { data.f[0] = 0.0f; data.f[1] = 0.0f; data.f[2] = 0.0f; data.f[3] = 1.0f; }
	Vector4(const Vector4& other) { memcpy(data.f, other.data.f, 4*4); }
	Vector4(const Vector3& other);
	Vector4(float* data) { memcpy(this->data.f, data, 4*4); }
	Vector4(float x, float y, float z, float w) { data.f[0] = x; data.f[1] = y; data.f[2] = z; data.f[3] = w; }
	const float* toArray() const { return data.f; }
	float* toArray() { return data.f; }
	float getX() const { return data.f[0]; }
	float getY() const { return data.f[1]; }
	float getZ() const { return data.f[2]; }
	float getW() const { return data.f[3]; }
	float operator[](unsigned int index) const { return data.f[index]; }
	void mirrorYZ() { float y = data.f[1]; data.f[1] = data.f[2]; data.f[2] = y; }
	void scale(float x, float y, float z) { data.f[0] *= x; data.f[1] *= y; data.f[2] *= z; }
	void scale(float f) { data.f[0] *= f; data.f[1] *= f; data.f[2] *= f; }
	Vector4& operator=(const Vector4& rhv);
	Vector4& operator+=(const Vector4& rhv);
	const Vector4 operator+(const Vector4& rhv) const { return Vector4(*this) += rhv; }
	Vector4& operator-=(const Vector4& rhv);
	const Vector4 operator-(const Vector4& rhv) const { return Vector4(*this) -= rhv; }
	Vector4& operator*=(float rhv);
	const Vector4 operator*(float rhv) const { return Vector4(*this) *= rhv; }
	const Vector4 operator-() const;
	float dot(const Vector4& rhv) const;

private:
	Vec4SF data;
};

#endif /* VECTOR4_H_ */
