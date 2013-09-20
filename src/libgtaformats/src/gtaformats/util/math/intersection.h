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

#ifndef INTERSECTION_H_
#define INTERSECTION_H_

#include "Vector3.h"
#include <cmath>
#include <algorithm>


bool IntersectAABoxSphere (
		float sx, float sy, float sz, float sr,
		float bx1, float by1, float bz1,
		float bx2, float by2, float bz2
);

inline bool IntersectSphereSphere (
		float x1, float y1, float z1, float r1,
		float x2, float y2, float z2, float r2
) {
	float xd = x2-x1;
	float yd = y2-y1;
	float zd = z2-z1;
	float rLen = r1+r2;
	return (xd*xd + yd*yd + zd*zd) <= rLen*rLen;
}

inline float DistanceRayPoint (
		const Vector3& rayStart, const Vector3& rayDir,
		const Vector3& pos
) {
	float r = -rayDir * (rayStart - pos) / rayDir.dot(rayDir);
	Vector3 rayClosestPoint = rayStart + rayDir * std::max(0.0f, r);
	return (rayClosestPoint - pos).length();
}

inline bool IntersectRaySphereSimple (
		const Vector3& rayStart, const Vector3& rayDir,
		const Vector3& sphereCenter, float sphereRadius
) {
	float distCenter = DistanceRayPoint(rayStart, rayDir, sphereCenter);
	return distCenter <= sphereRadius;
}

bool IntersectRaySphere (
		const Vector3& rayStart, const Vector3& rayDir,
		const Vector3& sphereCenter, float sphereRadius,
		Vector3& pointA, Vector3& pointB
);

inline bool IntersectRayPlane (
		const Vector3& rayStart, const Vector3& rayDir,
		const Vector3& planeN, const Vector3& planePoint,
		float& rayR, float& numerator
) {
	numerator = planeN.dot(planePoint-rayStart);
	float denominator = planeN.dot(rayDir);

	if (denominator == 0.0f) {
		// If numerator==0.0f, the ray lies completely inside the plane in which the triangle is contained
		return false;
	}

	rayR = numerator/denominator;
	return rayR >= 0.0f;
}

unsigned int IntersectRayAABox (
		const Vector3& rayStart, const Vector3& rayDir,
		const Vector3& boxMin, const Vector3& boxMax,
		Vector3 intersectionPoints[2],
		float intersectionRayR[2]
);

bool IntersectRayBox (
		const Vector3& rayStart, const Vector3& rayDir,
		const Vector3& boxOrigin, const Vector3& boxExtX, const Vector3& boxExtY, const Vector3& boxExtZ,
		float& rMin, float& rMax
);

bool IntersectRayTriangle (
		const Vector3& rayStart, const Vector3& rayDir,
		const Vector3& v0, const Vector3& v1, const Vector3& v2,
		float& s, float& t,
		float& rayR
);


#endif /* INTERSECTION_H_ */
