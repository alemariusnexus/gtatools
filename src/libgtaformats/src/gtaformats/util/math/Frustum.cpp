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

#include "Frustum.h"



Frustum::Frustum(float left, float right, float top, float bottom, float nnear, float ffar,
		const Vector3& pos, const Vector3& frontDir, const Vector3& upDir)
		: left(left), right(right), top(top), bottom(bottom), nnear(nnear), ffar(ffar), projMatValid(false)
{
	rightDir = frontDir.cross(upDir);
	rightDir.normalize();

	setDirection(frontDir, upDir);
	setPosition(pos);
}


void Frustum::calculateProjectionMatrix()
{
	if (projMatValid)
		return;

	/*projMat = Matrix4 (
		2*nnear/(right-left),		0,							0,								0,
		0,							2*nnear/(top-bottom),		0, 								0,
		(right+left)/(right-left),	(top+bottom)/(top-bottom),	(-(ffar+nnear))/(ffar-nnear),		-1,
		0,							0,							(-2*ffar*nnear)/(ffar-nnear),		0
	);*/

	projMat = Matrix4 (
		2*nnear/(right-left),		0,							0,								0,
		0,							2*nnear/(top-bottom),		0, 								0,
		(right+left)/(right-left),	(top+bottom)/(top-bottom),	-1,								-1,
		0,							0,							-2*nnear,						0
	);

	projMatValid = true;
}


void Frustum::setDistances(float l, float r, float t, float b, float n, float f)
{
	left = l;
	right = r;
	top = t;
	bottom = b;
	nnear = n;
	ffar = f;

	projMatValid = false;

	setDirection(frontDir, upDir);
}


void Frustum::setDirection(const Vector3& front, const Vector3& up)
{
	frontDir = front;
	upDir = up;

	rightDir = front.cross(up);
	rightDir.normalize();

	Vector3 leftPlaneDir = frontDir*nnear + rightDir*left;
	Vector3 rightPlaneDir = frontDir*nnear + rightDir*right;
	Vector3 topPlaneDir = frontDir*nnear + upDir*top;
	Vector3 bottomPlaneDir = frontDir*nnear + upDir*bottom;

	nLeft = leftPlaneDir.cross(upDir);
	nRight = upDir.cross(rightPlaneDir);
	nTop = topPlaneDir.cross(rightDir);
	nBottom = rightDir.cross(bottomPlaneDir);

	nLeft.normalize();
	nRight.normalize();
	nTop.normalize();
	nBottom.normalize();

	pNear = pos + frontDir*nnear;
	pFar = pos + frontDir*ffar;
}


void Frustum::setPosition(const Vector3& pos)
{
	this->pos = pos;

	pNear = pos + frontDir*nnear;
	pFar = pos + frontDir*ffar;
}


Frustum::PositionalRelationship Frustum::computePoint(const Vector3& point) const
{
	Vector3 pointMinusPos = point - pos;

	bool intersect = false;

	// Near plane
	float d = (point - pNear) * frontDir;

	if (d <= 0.0f) {
		if (d == 0.0f)
			intersect = true;
		else
			return Outside;
	}

	// Left plane
	d = (pointMinusPos) * nLeft;

	if (d <= 0.0f) {
		if (d == 0.0f)
			intersect = true;
		else
			return Outside;
	}

	// Right plane
	d = (pointMinusPos) * nRight;

	if (d <= 0.0f) {
		if (d == 0.0f)
			intersect = true;
		else
			return Outside;
	}

	// Top plane
	d = (pointMinusPos) * nTop;

	if (d <= 0.0f) {
		if (d == 0.0f)
			intersect = true;
		else
			return Outside;
	}

	// Bottom plane
	d = (pointMinusPos) * nBottom;

	if (d <= 0.0f) {
		if (d == 0.0f)
			intersect = true;
		else
			return Outside;
	}

	// Far plane
	d = (point - pFar) * frontDir;

	if (d >= 0.0f) {
		if (d == 0.0f)
			intersect = true;
		else
			return Outside;
	}


	if (intersect)
		return Intersecting;
	else
		return Inside;
}


Frustum::PositionalRelationship Frustum::computeSphere(const Vector3& center, float radius) const
{
	float mRadius = -radius;

	Vector3 pointMinusPos = center - pos;

	bool intersect = false;

	// Near plane
	float d = (center - pNear) * frontDir;

	if (d < mRadius) {
		return Outside;
	} else if (d < radius) {
		intersect = true;
	}

	// Left plane
	d = (pointMinusPos) * nLeft;

	if (d < mRadius) {
		return Outside;
	} else if (d < radius) {
		intersect = true;
	}

	// Right plane
	d = (pointMinusPos) * nRight;

	if (d < mRadius) {
		return Outside;
	} else if (d < radius) {
		intersect = true;
	}

	// Top plane
	d = (pointMinusPos) * nTop;

	if (d < mRadius) {
		return Outside;
	} else if (d < radius) {
		intersect = true;
	}

	// Bottom plane
	d = (pointMinusPos) * nBottom;

	if (d < mRadius) {
		return Outside;
	} else if (d < radius) {
		intersect = true;
	}

	// Far plane
	d = (center - pFar) * frontDir;

	if (d > radius) {
		return Outside;
	} else if (d > mRadius) {
		intersect = true;
	}

	if (intersect)
		return Intersecting;
	else
		return Inside;
}
