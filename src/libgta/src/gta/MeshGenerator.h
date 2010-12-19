/*
 * MeshGenerator.h
 *
 *  Created on: 17.12.2010
 *      Author: alemariusnexus
 */

#ifndef MESHGENERATOR_H_
#define MESHGENERATOR_H_

#include <gtaformats/util/math/Vector3.h>


class MeshGenerator {
public:
	void createBox(float*& vertices, int& vertexCount, int32_t*& indices, int& indexCount, const Vector3& min,
			const Vector3& max);
    void createSphere(float*& vertices, int& vertexCount, int32_t*& indices, int& indexCount, float radius,
    		int slices, int stacks);
};

#endif /* MESHGENERATOR_H_ */
