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

#ifndef DFFMODEL_H_
#define DFFMODEL_H_

#include <gtaformats/config.h>
#include "DFFGeometry.h"
#include "DFFFrame.h"
#include <vector>
#include <algorithm>

using std::vector;
using std::find;




/**	\brief A mesh structure as loaded by the DFFLoader class.
 *
 * 	\note DFF is still a very unexplored file format, so this class may always be subject to major extensions,
 * 	changes and even removes in the API, so it's still not a very stable interface. All of the following
 * 	description therefore is not guaranteed to be correct and may always change, but it's what the DFFLoader
 * 	class currently tries to extract from DFF:
 *
 * 	A DFFMesh by itself represents a complete three-dimensional mesh. It consists of a hierarchy of DFFFrames,
 * 	which hold 3D translation data, and a list of DFFGeometries. The latter are the actual containers of mesh
 * 	data. They contain the vertex data, flags and other properties, and each of them may refer to one of the
 * 	frames defined in it's DFFMesh, which positions the geometry relative to the mesh origin. The DFFGeometry
 * 	itself consists of multiple aggregated classes which hold the vertex indices, texturing data and so on,
 * 	which is documented along with the respective classes.
 *
 * 	@see DFFGeometry
 * 	@see DFFFrame
 * 	@see DFFLoader, the class which loads DFFMeshes from DFF files.
 */
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
	uint32_t getGeometryCount() const { return geometries.size(); }
	int32_t getBoneCount() const { return boneCount; }
	GeometryIterator getGeometryBegin() { return geometries.begin(); }
	ConstGeometryIterator getGeometryBegin() const { return geometries.begin(); }
	GeometryIterator getGeometryEnd() { return geometries.end(); }
	ConstGeometryIterator getGeometryEnd() const { return geometries.end(); }
	DFFGeometry* getGeometry(uint32_t idx);
	const DFFGeometry* getGeometry(uint32_t idx) const;
	DFFGeometry* getGeometry(const CString& name);
	const DFFGeometry* getGeometry(const CString& name) const;
	void addGeometry(DFFGeometry* geom) { geometries.push_back(geom); geom->reparent(this); }
	void removeGeometry(DFFGeometry* geom);
	void removeGeometry(uint32_t index) { removeGeometry(geometries[index]); }
	void removeGeometry(const CString& name) { removeGeometry(getGeometry(name)); }
	void removeGeometries();
	DFFFrame* getRootFrame() { return &rootFrame; }
	const DFFFrame* getRootFrame() const { return &rootFrame; }
	int32_t indexOf(DFFGeometry* geom) const
	{
		ConstGeometryIterator it = find(geometries.begin(), geometries.end(), geom);
		return it == geometries.end() ? -1 : it-geometries.begin();
	}

	void mirrorUVHorizontal();

private:
	DFFFrame rootFrame;
	vector<DFFGeometry*> geometries;
	int32_t boneCount;
};

#endif /* DFFMODEL_H_ */
