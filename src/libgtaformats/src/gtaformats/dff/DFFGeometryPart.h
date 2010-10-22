/*
	Copyright 2010 David "Alemarius Nexus" Lerch

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
 */

#ifndef DFFGEOMETRYPART_H_
#define DFFGEOMETRYPART_H_

#include "../gf_config.h"
#include "DFFMaterial.h"

class DFFGeometry;


class DFFGeometryPart {
private:
	friend class DFFLoader;
	friend class DFFGeometry;

public:
	DFFGeometryPart(int32_t ic, int32_t* indices);
	DFFGeometryPart() : indexCount(0), indices(new int32_t[0]), material(NULL), geometry(NULL) {}
	DFFGeometryPart(const DFFGeometryPart& other);
	~DFFGeometryPart();
	int32_t getIndexCount() const { return indexCount; }
	int32_t* getIndices() const { return indices; }
	DFFMaterial* getMaterial() const { return material; }
	void setIndices(int32_t count, int32_t* indices)
			{ delete[] indices; this->indices = indices; indexCount = count; }
	void setMaterial(DFFMaterial* mat);
	DFFGeometry* getGeometry() { return geometry; }
	const DFFGeometry* getGeometry() const { return geometry; }

private:
	void reparent(DFFGeometry* geom);

private:
	int32_t indexCount;
	int32_t* indices;
	DFFMaterial* material;
	DFFGeometry* geometry;
};

#endif /* DFFGEOMETRYPART_H_ */
