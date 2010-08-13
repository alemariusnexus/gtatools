/*
 * Vector3.h
 *
 *  Created on: 06.08.2010
 *      Author: alemariusnexus
 */

#ifndef VECTOR3_H_
#define VECTOR3_H_

#include <gf_config.h>
#include <cstring>


class Vector3
{
public:
	Vector3() : data(new float[3]) { data[0] = 0.0f; data[1] = 0.0f; data[2] = 0.0f; }
	Vector3(float* data) : data(data) {}
	Vector3(float x, float y, float z) : data(new float[3]) { data[0] = x; data[1] = y; data[2] = z; }
	const float* toArray() const { return data; }
	float* toArray() { return data; }
	float getX() const { return data[0]; }
	float getY() const { return data[1]; }
	float getZ() const { return data[2]; }
	float operator[](unsigned int index) { return data[index]; }
	void mirrorYZ() { float y = data[1]; data[1] = data[2]; data[2] = y; }
	void scale(float x, float y, float z) { data[0] *= x; data[1] *= y; data[2] *= z; }
	void scale(float f) { data[0] *= f; data[1] *= f; data[2] *= f; } // TODO Consider SIMD optimizing

private:
	float* data;
};

#endif /* VECTOR3_H_ */
