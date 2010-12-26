/*
 * COLBox.h
 *
 *  Created on: 31.10.2010
 *      Author: alemariusnexus
 */

#ifndef COLBOX_H_
#define COLBOX_H_

#include "../config.h"
#include "../util/math/Vector3.h"
#include "COLSurface.h"


class COLBox {
	friend class COLLoader;

public:
	COLBox(const Vector3& vmin, const Vector3& vmax, const COLSurface& surface) : vmin(vmin), vmax(vmax), surface(surface) {}
	COLBox(const COLBox& other) : vmin(other.vmin), vmax(other.vmax), surface(other.surface) {}
	COLBox() {}
	const Vector3& getMinimum() const { return vmin; }
	Vector3& getMinimum() { return vmin; }
	const Vector3& getMaximum() const { return vmax; }
	Vector3& getMaximum() { return vmax; }
	const COLSurface& getSurface() const { return surface; }
	COLSurface& getSurface() { return surface; }
	void setMinimum(const Vector3& vec) { vmin = vec; }
	void setMaximum(const Vector3& vec) { vmax = vec; }
	void setSurface(const COLSurface& surface) { this->surface = surface; }

private:
	Vector3 vmin;
	Vector3 vmax;
	COLSurface surface;
};

#endif /* COLBOX_H_ */
