/*
	Copyright 2010-2013 David "Alemarius Nexus" Lerch

	This file is part of libgta.

	libgta is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	libgta is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with libgta.  If not, see <http://www.gnu.org/licenses/>.

	Additional permissions are granted, which are listed in the file
	GPLADDITIONS.
 */

#ifndef FRUSTUM_H_
#define FRUSTUM_H_

#include "Vector3.h"
#include "Matrix4.h"


class Frustum
{
public:
	enum PositionalRelationship
	{
		Inside,
		Outside,
		Intersecting
	};

public:
	Frustum(float left, float right, float top, float bottom, float nnear, float ffar,
			const Vector3& pos = Vector3::Zero, const Vector3& frontDir = Vector3::NegativeUnitZ,
			const Vector3& upDir = Vector3::UnitY);
	Frustum(const Frustum& other) { memcpy(this, &other, sizeof(Frustum)); }
	float getLeftDistance() const { return left; }
	float getRightDistance() const { return right; }
	float getTopDistance() const { return top; }
	float getBottomDistance() const { return bottom; }
	float getNearDistance() const { return nnear; }
	float getFarDistance() const { return ffar; }
	const Vector3& getPosition() const { return pos; }
	const Vector3& getFrontDirection() const { return frontDir; }
	const Vector3& getUpDirection() const { return upDir; }
	const Vector3& getRightDirection() const { return rightDir; }
	void setDistances(float l, float r, float t, float b, float n, float f);
	void setLeftDistance(float l) { left = l; setDirection(frontDir, upDir); projMatValid = false; }
	void setRightDistance(float r) { right = r; setDirection(frontDir, upDir); projMatValid = false; }
	void setTopDistance(float t) { top = t; setDirection(frontDir, upDir); projMatValid = false; }
	void setBottomDistance(float b) { bottom = b; setDirection(frontDir, upDir); projMatValid = false; }
	void setNearDistance(float n) { nnear = n; setDirection(frontDir, upDir); projMatValid = false; }
	void setFarDistance(float f) { ffar = f; setDirection(frontDir, upDir); projMatValid = false; }
	void setDirection(const Vector3& front, const Vector3& up);
	void setPosition(const Vector3& pos);
	const Matrix4& getProjectionMatrix() { calculateProjectionMatrix(); return projMat; }

	PositionalRelationship computePoint(const Vector3& point) const;
	PositionalRelationship computeSphere(const Vector3& center, float radius) const;

private:
	void calculateProjectionMatrix();

private:
	Vector3 pos, frontDir, upDir, rightDir;
	float left, right;
	float top, bottom;
	float nnear, ffar;

	Vector3 nLeft, nRight;
	Vector3 nTop, nBottom;

	Vector3 pNear, pFar;

	Matrix4 projMat;
	bool projMatValid;
};

#endif /* FRUSTUM_H_ */
