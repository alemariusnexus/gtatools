/*
 * project.cpp
 *
 *  Created on: 18.08.2011
 *      Author: alemariusnexus
 */

#include "project.h"


bool Project(const Vector3& objPos, const Matrix4& modelMatrix, const Matrix4& projMatrix,
		int viewX, int viewY, int viewW, int viewH, Vector3& winPos)
{
	// Apply model and projection matrix
	Vector4 clipPos = projMatrix * modelMatrix * objPos;
	float cw = clipPos.getW();

	if (cw == 0.0f)
		return false;

	// Perform perspective division
	Vector3 normDevPos(clipPos.getX() / cw, clipPos.getY() / cw, clipPos.getZ() / cw);

	winPos = Vector3 (
			viewX + ((normDevPos.getX() + 1.0f) / 2.0f) * viewW,
			viewY + ((normDevPos.getY() + 1.0f) / 2.0f) * viewH,
			(normDevPos.getZ() + 1.0f) / 2.0f
	);

	return true;
}


bool UnProject(const Vector3& winPos, const Matrix4& modelMatrix, const Matrix4& projMatrix,
		int viewX, int viewY, int viewW, int viewH, Vector3& objPos)
{
	// Calculate the inverse matrix
	Matrix4 invPM = projMatrix * modelMatrix;
	invPM.invert();

	Vector3 normDevPos (
			((winPos.getX() - viewX) / viewW) * 2.0f - 1.0f,
			((winPos.getY() - viewY) / viewH) * 2.0f - 1.0f,
			2.0f * winPos.getZ() - 1.0f
	);

	Vector4 tmpObjPos(invPM * normDevPos);
	float ow = tmpObjPos.getW();

	if (ow == 0.0f)
		return false;

	objPos = Vector3(tmpObjPos.getX() / ow, tmpObjPos.getY() / ow, tmpObjPos.getZ() / ow);
	return true;
}
