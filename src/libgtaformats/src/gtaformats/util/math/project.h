/*
 * project.h
 *
 *  Created on: 18.08.2011
 *      Author: alemariusnexus
 */

#ifndef PROJECT_H_
#define PROJECT_H_

#include "Matrix4.h"


bool Project(const Vector3& objPos, const Matrix4& modelMatrix, const Matrix4& projMatrix,
		int viewX, int viewY, int viewW, int viewH, Vector3& winPos);
bool UnProject(const Vector3& winPos, const Matrix4& modelMatrix, const Matrix4& projMatrix,
		int viewX, int viewY, int viewW, int viewH, Vector3& objPos);

#endif /* PROJECT_H_ */
