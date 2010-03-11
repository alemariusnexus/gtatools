/*
 * DffModel.h
 *
 *  Created on: 04.01.2010
 *      Author: alemariusnexus
 */

#ifndef DFFMODEL_H_
#define DFFMODEL_H_

#include "../gf_config.h"
#include "DFFGeometry.h"
#include "DFFFrame.h"




class DFFMesh {
	friend class DFFLoader;

public:
	~DFFMesh();
	int32_t getFrameCount() const { return frameCount; }
	DFFFrame** getFrames() const { return frames; }
	DFFFrame* getFrame(int32_t idx) const { return frames[idx]; }
	int32_t getGeometryCount() const { return geometryCount; }
	DFFGeometry** getGeometries() const { return geometries; }
	DFFGeometry* getGeometry(int32_t idx) const { return geometries[idx]; }
	int32_t indexOf(DFFFrame* frame);

	void mirrorYZ();

private:
	DFFMesh() {};

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
