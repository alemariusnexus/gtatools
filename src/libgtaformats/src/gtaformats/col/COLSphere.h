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

#ifndef COLSPHERE_H_
#define COLSPHERE_H_

#include <gtaformats/config.h>
#include "../util/math/Vector3.h"
#include "COLSurface.h"


class COLSphere {
	friend class COLLoader;

public:
	COLSphere(const Vector3& center, float radius, const COLSurface& surface)
			: center(center), radius(radius), surface(surface) {}
	COLSphere(const COLSphere& other) : center(other.center), radius(other.radius), surface(other.surface) {}
	COLSphere() : radius(0.0f) {}
	const Vector3& getCenter() const { return center; }
	Vector3& getCenter() { return center; }
	float getRadius() const { return radius; }
	const COLSurface& getSurface() const { return surface; }
	COLSurface& getSurface() { return surface; }
	void setCenter(const Vector3& center) { this->center = center; }
	void setRadius(float radius) { this->radius = radius; }
	void setSurface(const COLSurface& surface) { this->surface = surface; }

private:
	Vector3 center;
	float radius;
	COLSurface surface;
};

#endif /* COLSPHERE_H_ */
