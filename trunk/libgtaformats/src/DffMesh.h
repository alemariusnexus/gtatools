/*
 * DffModel.h
 *
 *  Created on: 04.01.2010
 *      Author: alemariusnexus
 */

#ifndef DFFMODEL_H_
#define DFFMODEL_H_

#include "gf_config.h"
#include "DFFGeometry.h"
#include "DFFFrame.h"




class DffMesh {
	friend class DffLoader;

public:
	~DffMesh();
	int32_t getFrameCount() const { return frameCount; }
	DFFFrame** getFrames() const { return frames; }
	int32_t getGeometryCount() const { return geometryCount; }
	DFFGeometry** getGeometries() const { return geometries; }

	void mirrorYZ();

private:
	DffMesh() {};

private:
	int32_t frameCount;
	DFFFrame** frames;
	int32_t geometryCount;
	DFFGeometry** geometries;
	/*int32_t vertexCount;
	float* vertexPositions;
	float* vertexNormals;
	uint8_t* vertexColors;
	int8_t textureCoordinateSetCount;
	float* textureCoordinateSets;
	int32_t faceIndexCount;
	int32_t* faceIndices;
	int32_t* submeshFaceIndexCounts;
	int32_t submeshCount;
	DffBoundingSphere* bounds;
	bool facesUseTriStrips;
	DffMaterial** materials;
	int32_t materialCount;
	int32_t frameCount;
	DffFrame** frames;*/
};

#endif /* DFFMODEL_H_ */
