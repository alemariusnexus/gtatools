/*
 * COLFace.h
 *
 *  Created on: 31.10.2010
 *      Author: alemariusnexus
 */

#ifndef COLFACE_H_
#define COLFACE_H_

#include "../config.h"
#include "COLSurface.h"
#include <cstring>


class COLFace {
	friend class COLLoader;

public:
	COLFace(const uint32_t indices[3], const COLSurface& surface)
			: surface(surface) { memcpy(this->indices, indices, sizeof(indices)); }
	COLFace(const COLFace& other)
			: surface(other.surface) { memcpy(indices, other.indices, sizeof(indices)); }
	COLFace() { memset(indices, 0, 12); }
	const uint32_t* getIndices() const { return indices; }
	uint32_t* getIndices() { return indices; }
	const COLSurface& getSurface() const { return surface; }
	COLSurface& getSurface() { return surface; }
	void setIndices(const uint32_t indices[3]) { memcpy(this->indices, indices, sizeof(indices)); }
	void setIndices(uint32_t i0, uint32_t i1, uint32_t i2) { indices[0]=i0; indices[1]=i1; indices[2]=i2; }
	void setSurface(const COLSurface& surface) { this->surface = surface; }

private:
	uint32_t indices[3];
	COLSurface surface;
};

#endif /* COLFACE_H_ */
