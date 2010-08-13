/*
 * Matrix3.h
 *
 *  Created on: 06.08.2010
 *      Author: alemariusnexus
 */

#ifndef MATRIX3_H_
#define MATRIX3_H_

#include <gf_config.h>


class Matrix3
{
public:
	Matrix3();
	Matrix3(const Matrix3& other);
	Matrix3(float* data) : data(data) {}
	const float* toArray() const { return data; }
	float* toArray() { return data; }
	float operator[](unsigned int index) { return data[index]; }

private:
	float* data;
};

#endif /* MATRIX3_H_ */
