/*
 * COLSphere.h
 *
 *  Created on: 31.10.2010
 *      Author: alemariusnexus
 */

#ifndef COLSPHERE_H_
#define COLSPHERE_H_

#include "../config.h"
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
