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

#include "../gf_config.h"
#include "DFFGeometryPart.h"
#include "DFFMaterial.h"
#include "DFFFrame.h"
#include <vector>

using std::vector;


#define GEOMETRY_FLAG_TRISTRIP (1<<0)
#define GEOMETRY_FLAG_POSITIONS (1<<1)
#define GEOMETRY_FLAG_TEXCOORDS (1<<2)
#define GEOMETRY_FLAG_COLORS (1<<3)
#define GEOMETRY_FLAG_NORMALS (1<<4)
#define GEOMETRY_FLAG_UNKNOWN1 (1<<5)
#define GEOMETRY_FLAG_UNKNOWN2 (1<<6)
#define GEOMETRY_FLAG_MULTIPLEUVSETS (1<<7)


struct DFFBoundingSphere {
	float x;
	float y;
	float z;
	float radius;
};


class DFFGeometry {
public:
	typedef vector<DFFMaterial*>::iterator MaterialIterator;
	typedef vector<DFFMaterial*>::const_iterator ConstMaterialIterator;
	typedef vector<DFFGeometryPart*>::iterator PartIterator;
	typedef vector<DFFGeometryPart*>::const_iterator ConstPartIterator;

private:
	friend class DFFLoader;

public:
	DFFGeometry(int32_t numVertices, float* vertices, float* normals = NULL, float* uvCoords = NULL,
			int8_t uvSetCount = 0, uint8_t* vertexColors = NULL);
	DFFGeometry(const DFFGeometry& other);
	~DFFGeometry();

	bool isTriangleStripFormat() const { return (flags & GEOMETRY_FLAG_TRISTRIP) != 0; }
	int16_t getFlags() const { return flags; }
	int8_t getUVSetCount() const { return uvSetCount; }
	int32_t getVertexCount() const { return vertexCount; }
	int32_t getFrameCount() const { return frameCount; }
	float getAmbientLight() const { return ambientLight; }
	float getDiffuseLight() const { return diffuseLight; }
	float getSpecularLight() const { return specularLight; }
	uint8_t* getVertexColors() { return vertexColors; }
	const uint8_t* getVertexColors() const { return vertexColors; }
	float* getUVCoordSets() { return uvCoordSets; }
	const float* getUVCoordSets() const { return uvCoordSets; }
	float* getUVCoordSet(uint8_t idx = 0) { return uvCoordSets+idx*vertexCount; } // TODO
	const float* getUVCoordSet(uint8_t idx = 0) const { return uvCoordSets+idx*vertexCount; } // TODO
	const DFFBoundingSphere* getBounds() const { return bounds; }
	DFFBoundingSphere* getBounds() { return bounds; }
	float* getVertices() { return vertices; }
	const float* getVertices() const { return vertices; }
	float* getNormals() { return normals; }
	const float* getNormals() const { return normals; }
	int32_t getMaterialCount() const { return materials.size(); }
	int32_t getPartCount() const { return parts.size(); }
	int32_t indexOf(DFFMaterial* mat) const;
	DFFFrame* getAssociatedFrame() { return associatedFrame; }
	const DFFFrame* getAssociatedFrame() const { return associatedFrame; }
	void setFlags(int16_t flags) { this->flags = flags; }
	void setFrameCount(int32_t fc) { frameCount = fc; }
	void setAmbientLight(float al) { ambientLight = al; }
	void setDiffuseLight(float dl) { diffuseLight = dl; }
	void setSpecularLight(float sl) { specularLight = sl; }
	void setBounds(DFFBoundingSphere* bounds) { this->bounds = bounds; }
	void setVertices(int32_t numVertices, float* vertices, float* normals = NULL, float* uvCoords = NULL,
			int8_t uvSetCount = 0, uint8_t* vertexColors = NULL);
	MaterialIterator getMaterialBegin() { return materials.begin(); }
	MaterialIterator getMaterialEnd() { return materials.end(); }
	ConstMaterialIterator getMaterialBegin() const { return materials.begin(); }
	ConstMaterialIterator getMaterialEnd() const { return materials.end(); }
	DFFMaterial* getMaterial(int index) { return materials[index]; }
	const DFFMaterial* getMaterial(int index) const { return materials[index]; }
	void addMaterial(DFFMaterial* material) { materials.push_back(material); }
	void removeMaterial(int index) { removeMaterial(materials[index]); }
	void removeMaterial(DFFMaterial* material);
	void removeMaterials();
	PartIterator getPartBegin() { return parts.begin(); }
	PartIterator getPartEnd() { return parts.end(); }
	ConstPartIterator getPartBegin() const { return parts.begin(); }
	ConstPartIterator getPartEnd() const { return parts.end(); }
	DFFGeometryPart* getPart(int index) { return parts[index]; }
	const DFFGeometryPart* getPart(int index) const { return parts[index]; }
	void addPart(DFFGeometryPart* part) { parts.push_back(part); }
	void removePart(int index) { removePart(parts[index]); }
	void removePart(DFFGeometryPart* part);
	void removeParts();
	void setAssociatedFrame(DFFFrame* frame) { associatedFrame = frame; }

	void mirrorYZ();
	void scale(float x, float y, float z);

private:
	int16_t flags;
	int8_t uvSetCount;
	int32_t vertexCount;
	int32_t frameCount;

	float ambientLight;
	float diffuseLight;
	float specularLight;
	uint8_t* vertexColors;
	float* uvCoordSets;
	DFFBoundingSphere* bounds;
	float* vertices;
	float* normals;

	vector<DFFMaterial*> materials;
	vector<DFFGeometryPart*> parts;

	DFFFrame* associatedFrame;
};

#endif /* DFFGEOMETRY_H_ */
