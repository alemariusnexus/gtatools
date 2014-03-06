/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

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

#ifndef COLOR4_H_
#define COLOR4_H_

#include <gtaformats/config.h>
#include "math/Vector4.h"


class Color4 {
public:
	Color4(float r, float g, float b, float a = 1.0f) : vec(Vector4(r, g, b, a)) {}
	Color4(const Color4& other) : vec(other.vec) {}
	float getRed() const { return vec.getX(); }
	float getGreen() const { return vec.getY(); }
	float getBlue() const { return vec.getZ(); }
	float getAlpha() const { return vec.getW(); }
	void getValue(float& r, float& g, float& b, float& a) const
			{ r = vec.getX(); g = vec.getY(); b = vec.getZ(); a = vec.getW(); }
	//void blend(const Color4& other) { vec = vec*vec.getW() + other.vec*(1.0f-vec.getW()); }
	void blend(const Color4& other, float otherRgbFactor, float thisRgbFactor, float otherAFactor, float thisAFactor);
	void blend(const Color4& other, float otherFactor, float thisFactor) { blend(other, otherFactor, thisFactor, otherFactor, thisFactor); }
	void blend(const Color4& other) { blend(other, other.getAlpha(), 1.0f-other.getAlpha()); }

private:
	Vector4 vec;
};

#endif /* COLOR4_H_ */
