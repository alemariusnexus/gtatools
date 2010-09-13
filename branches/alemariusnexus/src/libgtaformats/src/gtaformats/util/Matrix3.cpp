/*
 * Matrix3.cpp
 *
 *  Created on: 06.08.2010
 *      Author: alemariusnexus
 */

#include "Matrix3.h"
#include <cstring>


Matrix3::Matrix3()
		: data(new float[9])
{
	data[0] = 0.0f;
	data[1] = 0.0f;
	data[2] = 0.0f;
	data[3] = 0.0f;
	data[4] = 0.0f;
	data[5] = 0.0f;
	data[6] = 0.0f;
	data[7] = 0.0f;
	data[8] = 0.0f;
}


Matrix3::Matrix3(const Matrix3& other)
		: data(new float[9])
{
	memcpy(data, other.data, 9*4);
}
