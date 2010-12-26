/*
 * Color4.h
 *
 *  Created on: 17.12.2010
 *      Author: alemariusnexus
 */

#ifndef COLOR4_H_
#define COLOR4_H_

#include "../config.h"
#include "math/Vector4.h"


class Color4 {
public:
	Color4(float r, float g, float b, float a = 1.0f) : vec(Vector4(r, g, b, a)) {}
	Color4(const Color4& other) : vec(other.vec) {}
	float getRed() { return vec.getX(); }
	float getGreen() { return vec.getY(); }
	float getBlue() { return vec.getZ(); }
	float getAlpha() { return vec.getW(); }
	void getValue(float& r, float& g, float& b, float& a)
			{ r = vec.getX(); g = vec.getY(); b = vec.getZ(); a = vec.getW(); }
	void blend(const Color4& other) { vec = vec*vec.getW() + other.vec*(1.0f-vec.getW()); }

private:
	Vector4 vec;
};

#endif /* COLOR4_H_ */
