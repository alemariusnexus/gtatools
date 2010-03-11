/*
 * DFFGeometry.h
 *
 *  Created on: 12.02.2010
 *      Author: alemariusnexus
 */

#ifndef DFFGEOMETRY_H_
#define DFFGEOMETRY_H_

#include "../gf_config.h"
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
