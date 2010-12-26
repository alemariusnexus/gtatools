/*
 * COLShadowMesh.h
 *
 *  Created on: 31.10.2010
 *      Author: alemariusnexus
 */

#ifndef COLSHADOWMESH_H_
#define COLSHADOWMESH_H_

#include "../config.h"
#include "COLFace.h"


class COLShadowMesh {
	friend class COLLoader;

public:
	COLShadowMesh(uint32_t numVertices, uint32_t numFaces, float* vertices, COLFace* faces)
			: numVertices(numVertices), numFaces(numFaces), vertices(vertices), faces(faces) {}
	COLShadowMesh(const COLShadowMesh& other);
	uint32_t getVertexCount() const { return numVertices; }
	uint32_t getFaceCount() const { return numFaces; }
	const float* getVertices() const { return vertices; }
	float* getVertices() { return vertices; }
	const COLFace* getFaces() const { return faces; }
	COLFace* getFaces() { return faces; }
	void setVertices(uint32_t num, float* vertices) { numVertices = num; this->vertices = vertices; }
	void setFaces(uint32_t num, COLFace* faces) { numFaces = num; this->faces = faces; }

private:
	uint32_t numVertices;
	uint32_t numFaces;
	float* vertices;
	COLFace* faces;
};

#endif /* COLSHADOWMESH_H_ */
