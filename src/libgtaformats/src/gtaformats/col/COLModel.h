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

#ifndef COLMODEL_H_
#define COLMODEL_H_

#include <gtaformats/config.h>
#include "COLBox.h"
#include "COLSphere.h"
#include "COLFace.h"
#include "COLFaceGroup.h"
#include "COLBounds.h"
#include "COLShadowMesh.h"



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
	const char* getName() const { return name; }
	char* getName() { return name; }
	const COLBounds& getBounds() const { return bounds; }
	COLBounds& getBounds() { return bounds; }
	uint32_t getSphereCount() const { return numSpheres; }
	uint32_t getBoxCount() const { return numBoxes; }
	uint32_t getFaceCount() const { return numFaces; }
	uint32_t getFlags() const { return flags; }
	uint32_t getVertexCount() const { return numVertices; }
	uint32_t getFaceGroupCount() const { return numFaceGroups; }
	const COLShadowMesh* getShadowMesh() const { return shadowMesh; }
	COLShadowMesh* getShadowMesh() { return shadowMesh; }
	const COLSphere* getSpheres() const { return spheres; }
	COLSphere* getSpheres() { return spheres; }
	const COLBox* getBoxes() const { return boxes; }
	COLBox* getBoxes() { return boxes; }
	const float* getVertices() const { return vertices; }
	float* getVertices() { return vertices; }
	const COLFace* getFaces() const { return faces; }
	COLFace* getFaces() { return faces; }
	const COLFaceGroup* getFaceGroups() const { return faceGroups; }
	COLFaceGroup* getFaceGroups() { return faceGroups; }
	void setName(const char* name) { strncpy(this->name, name, 20); this->name[19] = '\0'; }
	void setBounds(const COLBounds& bounds) { this->bounds = bounds; }
	void setSpheres(uint32_t num, COLSphere* spheres) { numSpheres = num; this->spheres = spheres; }
	void setBoxes(uint32_t num, COLBox* boxes) { numBoxes = num; this->boxes = boxes; }
	void setVertices(uint32_t num, float* vertices) { numVertices = num; this->vertices = vertices; }
	void setFaces(uint32_t num, COLFace* faces) { numFaces = num; this->faces = faces; }
	void setFaceGroups(uint32_t num, COLFaceGroup* groups) { numFaceGroups = num; faceGroups = groups; }
	void setShadowMesh(COLShadowMesh* shadowMesh) { this->shadowMesh = shadowMesh; }

private:
	//COLVersion version;
	char name[20];
	COLBounds bounds;
	uint32_t numSpheres;
	uint32_t numBoxes;
	uint32_t numFaces;
	uint32_t flags;
	uint32_t numVertices;
	uint32_t numFaceGroups;
	COLSphere* spheres;
	COLBox* boxes;
	float* vertices;
	COLFace* faces;
	COLFaceGroup* faceGroups;
	COLShadowMesh* shadowMesh;
};

#endif /* COLMODEL_H_ */
