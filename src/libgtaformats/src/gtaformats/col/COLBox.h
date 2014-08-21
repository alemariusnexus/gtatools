/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#ifndef COLBOX_H_
#define COLBOX_H_

#include <gtaformats/config.h>
#include <nxcommon/math/Vector3.h>
#include "COLSurface.h"



#pragma pack(push, 1)


class COLBox {
	friend class COLLoader;

public:
	COLBox(const Vector3& vmin, const Vector3& vmax, const COLSurface& surface) : vmin(vmin), vmax(vmax), surface(surface) {}
	COLBox(const COLBox& other) : vmin(other.vmin), vmax(other.vmax), surface(other.surface) {}
	COLBox() {}
	Vector3 getMinimum() const { return vmin; }
	Vector3 getMaximum() const { return vmax; }
	COLSurface getSurface() const { return surface; }
	void setMinimum(const Vector3& vec) { vmin = vec; }
	void setMaximum(const Vector3& vec) { vmax = vec; }
	void setSurface(const COLSurface& surface) { this->surface = surface; }

private:
	Vector3 vmin;
	Vector3 vmax;
	COLSurface surface;
};


#pragma pack(pop)

#endif /* COLBOX_H_ */
