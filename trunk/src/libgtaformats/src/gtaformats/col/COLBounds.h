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
	COLBounds(const Vector3& vmin, const Vector3& vmax, const Vector3& center, float radius)
			: vmin(vmin), vmax(vmax), center(center), radius(radius) {}
	COLBounds(const COLBounds& other)
			: vmin(other.vmin), vmax(other.vmax), center(other.center), radius(other.radius) {}
	COLBounds() : radius(0.0f) {}
	const Vector3& getMinimum() const { return vmin; }
	Vector3& getMinimum() { return vmin; }
	const Vector3& getMaximum() const { return vmax; }
	Vector3& getMaximum() { return vmax; }
	const Vector3& getCenter() const { return center; }
	Vector3& getCenter() { return center; }
	float getRadius() const { return radius; }
	void setMinimum(const Vector3& vmin) { this->vmin = vmin; }
	void setMaximum(const Vector3& vmax) { this->vmax = vmax; }
	void setCenter(const Vector3& center) { this->center = center; }
	void setRadius(float radius) { this->radius = radius; }

private:
	Vector3 vmin;
	Vector3 vmax;
	Vector3 center;
	float radius;
};

#endif /* COLBOUNDS_H_ */
