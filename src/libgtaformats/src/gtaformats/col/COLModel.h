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

#ifndef COLMODEL_H_
#define COLMODEL_H_

#include <gtaformats/config.h>
#include <nxcommon/CString.h>
#include "COLBox.h"
#include "COLSphere.h"
#include "COLFace.h"
#include "COLFaceGroup.h"
#include "COLBounds.h"
#include "COLShadowMesh.h"
#include <vector>

using std::vector;




enum COLVersion
{
	ColEnd = 0,
	COL1 = 1,
	COL2 = 2,
	COL3 = 3
};



class COLModel {
	friend class COLLoader;

public:
	enum Flags
	{
		FlagNotEmpty = (1 << 1),
		FlagFaceGroups = (1 << 3),
		FlagShadowMesh = (1 << 4)
	};

public:
	COLModel();
	COLModel(const COLModel& other);
	~COLModel();

	CString getName() const { return name; }

	COLBounds& getBounds() { return bounds; }
	const COLBounds& getBounds() const { return bounds; }

	size_t getSphereCount() const { return spheres.size(); }
	size_t getBoxCount() const { return boxes.size(); }
	size_t getFaceCount() const { return faces.size(); }
	size_t getFlags() const { return flags; }
	size_t getVertexCount() const { return numVertices; }
	size_t getFaceGroupCount() const { return faceGroups.size(); }

	vector<COLSphere>& getSpheres() { return spheres; }
	const vector<COLSphere>& getSpheres() const { return spheres; }

	vector<COLBox>& getBoxes() { return boxes; }
	const vector<COLBox>& getBoxes() const { return boxes; }

	vector<COLFace>& getFaces() { return faces; }
	const vector<COLFace>& getFaces() const { return faces; }

	vector<COLFaceGroup>& getFaceGroups() { return faceGroups; }
	const vector<COLFaceGroup>& getFaceGroups() const { return faceGroups; }

	const COLShadowMesh* getShadowMesh() const { return shadowMesh; }
	COLShadowMesh* getShadowMesh() { return shadowMesh; }

	const float* getVertices() const { return vertices; }
	float* getVertices() { return vertices; }

	void setName(const CString& name) { this->name = name; }
	void setBounds(const COLBounds& bounds) { this->bounds = bounds; }

	void setVertices(uint32_t num, float* vertices) { numVertices = num; this->vertices = vertices; }

private:
	CString name;
	COLBounds bounds;
	uint32_t flags;
	uint32_t numVertices;

	vector<COLSphere> spheres;
	vector<COLBox> boxes;
	vector<COLFace> faces;
	vector<COLFaceGroup> faceGroups;

	float* vertices;
	COLShadowMesh* shadowMesh;
};

#endif /* COLMODEL_H_ */
