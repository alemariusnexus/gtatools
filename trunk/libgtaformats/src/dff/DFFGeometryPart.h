/*
 * DFFGeometryPart.h
 *
 *  Created on: 12.02.2010
 *      Author: alemariusnexus
 */

#ifndef DFFGEOMETRYPART_H_
#define DFFGEOMETRYPART_H_

#include "../config.h"
#include "DFFMaterial.h"


class DFFGeometryPart {
private:
	friend class DffLoader;

public:
	~DFFGeometryPart();
	int32_t getIndexCount() const { return indexCount; }
	int32_t* getIndices() const { return indices; }
	DFFMaterial* getMaterial() const { return material; }

private:
	int32_t indexCount;
	int32_t* indices;
	DFFMaterial* material;
};

#endif /* DFFGEOMETRYPART_H_ */
