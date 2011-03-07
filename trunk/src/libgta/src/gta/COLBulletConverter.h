/*
 * COLBulletConverter.h
 *
 *  Created on: 12.02.2011
 *      Author: alemariusnexus
 */

#ifndef COLBULLETCONVERTER_H_
#define COLBULLETCONVERTER_H_

#include <btBulletDynamicsCommon.h>
#include <gtaformats/gtacol.h>


class COLBulletConverter {
public:
	btCollisionShape* convert(const COLSphere& sphere);
	btCollisionShape* convert(const COLBox& box);
	btCollisionShape* convert(const float* vertices, int vertexCount, const COLFace* faces, int faceCount);
	btCollisionShape* convert(const COLModel& model);
};

#endif /* COLBULLETCONVERTER_H_ */
