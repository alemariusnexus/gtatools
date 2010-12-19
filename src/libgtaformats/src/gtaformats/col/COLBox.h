/*
 * COLBox.h
 *
 *  Created on: 31.10.2010
 *      Author: alemariusnexus
 */

#ifndef COLBOX_H_
#define COLBOX_H_

#include "../util/math/Vector3.h"
#include "COLSurface.h"


class COLBox {
	friend class COLLoader;

public:
	COLBox(const Vector3& min, const Vector3& max, const COLSurface& surface)
			: min(min), max(max), surface(surface) {}
	COLBox(const COLBox& other) : min(other.min), max(other.max), surface(other.surface) {}
	COLBox() {}
	const Vector3& getMinimum() const { return min; }
	Vector3& getMinimum() { return min; }
	const Vector3& getMaximum() const { return max; }
	Vector3& getMaximum() { return max; }
	const COLSurface& getSurface() const { return surface; }
	COLSurface& getSurface() { return surface; }
	void setMinimum(const Vector3& vec) { min = vec; }
	void setMaximum(const Vector3& vec) { max = vec; }
	void setSurface(const COLSurface& surface) { this->surface = surface; }

private:
	Vector3 min;
	Vector3 max;
	COLSurface surface;
};

#endif /* COLBOX_H_ */
