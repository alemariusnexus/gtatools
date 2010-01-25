/*
 * DffModel.h
 *
 *  Created on: 04.01.2010
 *      Author: alemariusnexus
 */

#ifndef DFFMODEL_H_
#define DFFMODEL_H_

#include "config.h"

struct DffBoundingSphere {
	float x;
	float y;
	float z;
	float radius;
};

struct DffTexture {
	char* textureName;
	char* maskName;
	uint16_t filterModeFlags;
};

struct DffMaterial {
	int32_t _unknown1;
	uint8_t colors[4];
	int32_t _unknown2;
	int32_t textureCount;
	float _unknown3;
	float _unknown4;
	float _unknown5;
	DffTexture** textures;
};

class DffMesh {
	friend class DffLoader;

public:
	~DffMesh();
	void scale(float factor);

	int32_t getVertexCount() { return vertexCount; }
	float* getVertexPositions() { return vertexPositions; }
	float* getVertexNormals() { return vertexNormals; }
	uint8_t* getVertexColors() { return vertexColors; }
	int8_t getTextureCoordinateSetCount() { return textureCoordinateSetCount; }
	float* getTextureCoordinateSets() { return textureCoordinateSets; }
	int32_t getFaceIndexCount() { return faceIndexCount; }
	int32_t* getFaceIndices() { return faceIndices; }
	int32_t* getSubmeshFaceIndexCounts() { return submeshFaceIndexCounts; }
	uint8_t getSubmeshCount() { return submeshCount; }
	DffBoundingSphere* getBounds() { return bounds; }
	bool isUsingTriangleStrips() { return facesUseTriStrips; }
	DffMaterial** getMaterials() { return materials; }
	int32_t getMaterialCount() { return materialCount; }

private:
	DffMesh();

private:
	int32_t vertexCount;
	float* vertexPositions;
	float* vertexNormals;
	uint8_t* vertexColors;
	int8_t textureCoordinateSetCount;
	float* textureCoordinateSets;
	int32_t faceIndexCount;
	int32_t* faceIndices;
	int32_t* submeshFaceIndexCounts;
	uint8_t submeshCount;
	DffBoundingSphere* bounds;
	bool facesUseTriStrips;
	DffMaterial** materials;
	int32_t materialCount;
};

#endif /* DFFMODEL_H_ */
