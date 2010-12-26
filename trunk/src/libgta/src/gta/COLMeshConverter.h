/*
 * COLMeshConverter.h
 *
 *  Created on: 17.12.2010
 *      Author: alemariusnexus
 */

#ifndef COLMESHCONVERTER_H_
#define COLMESHCONVERTER_H_

#include "config.h"
#include <gtaformats/col/COLModel.h>
#include "Mesh.h"


class COLMeshConverter {
public:
	Mesh* convert(const COLModel& model);

private:
	void getMaterialColors(uint8_t mat, uint8_t& r, uint8_t& g, uint8_t& b);
	void convertVertexModel(const COLModel& model, int& vertexCount, float*& vertices, uint8_t*& colors,
			int32_t*& indices);
};

#endif /* COLMESHCONVERTER_H_ */
