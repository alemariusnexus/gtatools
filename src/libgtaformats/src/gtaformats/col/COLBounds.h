/*
 * COLBounds.h
 *
 *  Created on: 31.10.2010
 *      Author: alemariusnexus
 */

#ifndef COLBOUNDS_H_
#define COLBOUNDS_H_

#include "../util/math/Vector3.h"


class COLBounds {
	friend class COLLoader;

public:
	COLBounds(const Vector3& min, const Vector3& max, const Vector3& center, float radius)
			: min(min), max(max), center(center), radius(radius) {}
	COLBounds(const COLBounds& other)
			: min(other.min), max(other.max), center(other.center), radius(other.radius) {}
	COLBounds() : radius(0.0f) {}
	const Vector3& getMinimum() const { return min; }
	Vector3& getMinimum() { return min; }
	const Vector3& getMaximum() const { return max; }
	Vector3& getMaximum() { return max; }
	const Vector3& getCenter() const { return center; }
	Vector3& getCenter() { return center; }
	float getRadius() const { return radius; }
	void setMinimum(const Vector3& min) { this->min = min; }
	void setMaximum(const Vector3& max) { this->max = max; }
	void setCenter(const Vector3& center) { this->center = center; }
	void setRadius(float radius) { this->radius = radius; }

private:
	Vector3 min;
	Vector3 max;
	Vector3 center;
	float radius;
};

#endif /* COLBOUNDS_H_ */
