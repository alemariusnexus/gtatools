/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of gtaformats.

	gtaformats is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	gtaformats is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with gtaformats.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#include "project.h"
#include <cstdio>



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

	/*const float* ma = invPM.toArray();
	printf("invPM (\n");
	printf("    %.3f   %.3f   %.3f   %.3f\n", ma[0], ma[1], ma[2], ma[3]);
	printf("    %.3f   %.3f   %.3f   %.3f\n", ma[4], ma[5], ma[6], ma[7]);
	printf("    %.3f   %.3f   %.3f   %.3f\n", ma[8], ma[9], ma[10], ma[11]);
	printf("    %.3f   %.3f   %.3f   %.3f\n", ma[12], ma[13], ma[14], ma[15]);
	printf(")\n");*/

	Vector3 normDevPos (
			((winPos.getX() - viewX) / viewW) * 2.0f - 1.0f,
			((winPos.getY() - viewY) / viewH) * 2.0f - 1.0f,
			2.0f * winPos.getZ() - 1.0f
	);

	Vector4 tmpObjPos(invPM * Vector4(normDevPos.getX(), normDevPos.getY(), normDevPos.getZ(), 1.0f));
	float ow = tmpObjPos.getW();

	if (ow == 0.0f)
		return false;

	objPos = Vector3(tmpObjPos.getX() / ow, tmpObjPos.getY() / ow, tmpObjPos.getZ() / ow);
	return true;
}
