/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef COLLISIONSPHERE_H_
#define COLLISIONSPHERE_H_

#include <gta/config.h>
#include <gtaformats/col/COLSphere.h>
#include <btBulletCollisionCommon.h>


class CollisionSphere
{
public:
	CollisionSphere(const Vector3& center, float radius) : center(center), radius(radius) {}
	CollisionSphere(const CollisionSphere& other) : center(other.center), radius(other.radius) {}
	CollisionSphere(const COLSphere& sphere) : center(sphere.getCenter()), radius(sphere.getRadius()) {}
	Vector3& getCenter() { return center; }
	const Vector3& getCenter() const { return center; }
	float getRadius() const { return radius; }
	void setCenter(const Vector3& c) { center = c; }
	void setRadius(float r) { radius = r; }

private:
	void updateBulletShape();

private:
	Vector3 center;
	float radius;
};

#endif /* COLLISIONSPHERE_H_ */
