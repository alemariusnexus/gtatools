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

#ifndef DFFGEOMETRY_H_
#define DFFGEOMETRY_H_

#include <gf_config.h>
#include "DFFGeometryPart.h"
#include "DFFMaterial.h"
#include "DFFFrame.h"



struct DFFBoundingSphere {
	float x;
	float y;
	float z;
	float radius;
};


class DFFGeometry {
private:
	friend class DFFLoader;

public:
	DFFGeometry() : vertexColors(NULL), uvCoordSets(NULL), vertices(NULL), normals(NULL) {}
	~DFFGeometry();

	bool isTriangleStripFormat() const { return triangleStrips; }
	int16_t getFlags() const { return flags; }
	int8_t getUVSetCount() const { return uvSetCount; }
	int32_t getVertexCount() const { return vertexCount; }
	int32_t getFrameCount() const { return frameCount; }
	float getAmbientLight() const { return ambientLight; }
	float getDiffuseLight() const { return diffuseLight; }
	float getSpecularLight() const { return specularLight; }
	uint8_t* getVertexColors() const { return vertexColors; }
	float** getUVCoordSets() const { return uvCoordSets; }
	float* getUVCoordSet(uint8_t idx = 0) const { return uvCoordSets[idx*vertexCount]; }
	const DFFBoundingSphere* getBounds() const { return &bounds; }
	float* getVertices() const { return vertices; }
	float* getNormals() const { return normals; }
	int32_t getMaterialCount() const { return materialCount; }
	DFFMaterial** getMaterials() const { return materials; }
	int32_t getPartCount() const { return partCount; }
	DFFGeometryPart** getParts() const { return parts; }
	int32_t indexOf(DFFMaterial* mat) const;
	DFFFrame* getAssociatedFrame() const { return associatedFrame; }
	//int32_t indexOf(DFFFrame* frame) const;

	void mirrorYZ();
	void scale(float x, float y, float z);

private:
	bool triangleStrips;
	int16_t flags;
	int8_t uvSetCount;
	int32_t vertexCount;
	int32_t frameCount;

	float ambientLight;
	float diffuseLight;
	float specularLight;
	uint8_t* vertexColors;
	float** uvCoordSets;
	DFFBoundingSphere bounds;
	float* vertices;
	float* normals;

	int32_t materialCount;
	DFFMaterial** materials;
	int32_t partCount;
	DFFGeometryPart** parts;

	DFFFrame* associatedFrame;
};

#endif /* DFFGEOMETRY_H_ */
