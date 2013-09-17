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

#ifndef COLFACE_H_
#define COLFACE_H_

#include <gtaformats/config.h>
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
