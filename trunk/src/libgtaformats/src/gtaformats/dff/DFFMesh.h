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

#ifndef DFFMODEL_H_
#define DFFMODEL_H_

#include "../gf_config.h"
#include "DFFGeometry.h"
#include "DFFFrame.h"
#include <vector>

using std::vector;




class DFFMesh {
private:
	friend class DFFLoader;

public:
	typedef vector<DFFGeometry*>::iterator GeometryIterator;
	typedef vector<DFFGeometry*>::const_iterator ConstGeometryIterator;

public:
	DFFMesh() {}
	DFFMesh(const DFFMesh& other);
	~DFFMesh();
	int32_t getGeometryCount() const { return geometries.size(); }
	GeometryIterator getGeometryBegin() { return geometries.begin(); }
	ConstGeometryIterator getGeometryBegin() const { return geometries.begin(); }
	GeometryIterator getGeometryEnd() { return geometries.end(); }
	ConstGeometryIterator getGeometryEnd() const { return geometries.end(); }
	DFFGeometry* getGeometry(int32_t idx);
	const DFFGeometry* getGeometry(int32_t idx) const;
	DFFGeometry* getGeometry(const char* name);
	const DFFGeometry* getGeometry(const char* name) const;
	void addGeometry(DFFGeometry* geom) { geometries.push_back(geom); geom->reparent(this); }
	void removeGeometry(DFFGeometry* geom);
	void removeGeometry(int32_t index) { removeGeometry(geometries[index]); }
	void removeGeometry(const char* name) { removeGeometry(getGeometry(name)); }
	void removeGeometries();
	DFFFrame* getRootFrame() { return &rootFrame; }
	const DFFFrame* getRootFrame() const { return &rootFrame; }

	void mirrorYZ();
	void scale(float x, float y, float z);
	void mirrorUVHorizontal();

private:
	DFFFrame rootFrame;
	vector<DFFGeometry*> geometries;
};

#endif /* DFFMODEL_H_ */
