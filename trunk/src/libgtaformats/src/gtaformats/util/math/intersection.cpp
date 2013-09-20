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


#include "intersection.h"
#include <cstdio>
#include <cstdlib>
#include <algorithm>

using std::min;
using std::max;




bool IntersectAABoxSphere(float sx, float sy, float sz, float sr,
		float bx1, float by1, float bz1,
		float bx2, float by2, float bz2)
{
	float distSq = 0;

	if (sx < bx1) {
		distSq += (bx1-sx) * (bx1-sx);
	} else if (sx > bx2) {
		distSq += (sx-bx2) * (sx-bx2);
	}

	if (sy < by1) {
		distSq += (by1-sy) * (by1-sy);
	} else if (sy > by2) {
		distSq += (sy-by2) * (sy-by2);
	}

	if (sz < bz1) {
		distSq += (bz1-sz) * (bz1-sz);
	} else if (sz > bz2) {
		distSq += (sz-bz2) * (sz-bz2);
	}

	return distSq <= sr*sr;
}


bool IntersectRaySphere (
		const Vector3& rayStart, const Vector3& rayDir,
		const Vector3& sphereCenter, float sphereRadius,
		Vector3& pointA, Vector3& pointB
) {
	Vector3 startToCenter = rayStart - sphereCenter;
	float stcDotMinusStc = startToCenter.dot(-startToCenter);
	float dirDotDir = rayDir.dot(rayDir);
	float a = startToCenter.dot(rayDir) * 2;

	float discriminant = (sphereRadius*sphereRadius + stcDotMinusStc) / dirDotDir
			+ a*a / (4* dirDotDir*dirDotDir);

	if (discriminant < 0.0f) {
		return false;
	}

	float subtrahend = a / (2*dirDotDir);

	float r1 = -subtrahend - sqrtf(discriminant);
	float r2 = -subtrahend + sqrtf(discriminant);

	if (r1 < 0.0f  &&  r2 < 0.0f) {
		return false;
	}

	pointA = rayStart + rayDir*r1;
	pointB = rayStart + rayDir*r2;

	return true;
}


unsigned int IntersectRayAABox (
		const Vector3& rayStart, const Vector3& rayDir,
		const Vector3& boxMin, const Vector3& boxMax,
		Vector3 intersectionPoints[2],
		float intersectionRayR[2]
) {
	unsigned int intersectCount = 0;

	float xMin = boxMin.getX();
	float yMin = boxMin.getY();
	float zMin = boxMin.getZ();
	float xMax = boxMax.getX();
	float yMax = boxMax.getY();
	float zMax = boxMax.getZ();

	float lenX = xMax-xMin;
	float lenY = yMax-yMin;
	float lenZ = zMax-zMin;

	float xRs = rayStart.getX();
	float yRs = rayStart.getY();
	float zRs = rayStart.getZ();

	float xRd = rayDir.getX();
	float yRd = rayDir.getY();
	float zRd = rayDir.getZ();

	if (xRd != 0.0f) {
		// X Minimum Plane
		float xMinR = (boxMin.getX()-xRs) / xRd;
		if (xMinR >= 0.0f) {
			Vector3 xMinPos = rayStart + rayDir * xMinR;
			float xMindy = xMinPos.getY() - yMin;
			if (xMindy >= 0.0f  &&  xMindy <= lenY) {
				float xMindz = xMinPos.getZ() - zMin;
				if (xMindz >= 0.0f  &&  xMindz <= lenZ) {
					intersectionPoints[intersectCount] = xMinPos;
					intersectionRayR[intersectCount++] = xMinR;
				}
			}
		}

		// X Maximum Plane
		float xMaxR = (boxMax.getX()-xRs) / xRd;
		if (xMaxR >= 0.0f) {
			Vector3 xMaxPos = rayStart + rayDir * xMaxR;
			float xMaxdy = xMaxPos.getY() - yMin;
			if (xMaxdy >= 0.0f  &&  xMaxdy <= lenY) {
				float xMaxdz = xMaxPos.getZ() - zMin;
				if (xMaxdz >= 0.0f  &&  xMaxdz <= lenZ) {
					intersectionPoints[intersectCount] = xMaxPos;
					intersectionRayR[intersectCount++] = xMaxR;
				}
			}
		}
	}

	if (yRd != 0.0f) {
		// Y Minimum Plane
		float yMinR = (boxMin.getY()-yRs) / yRd;
		if (yMinR >= 0.0f) {
			Vector3 yMinPos = rayStart + rayDir * yMinR;
			float yMindx = yMinPos.getX() - xMin;
			if (yMindx >= 0.0f  &&  yMindx <= lenX) {
				float yMindz = yMinPos.getZ() - zMin;
				if (yMindz >= 0.0f  &&  yMindz <= lenZ) {
					if (intersectCount == 2)
						return 1;

					intersectionPoints[intersectCount] = yMinPos;
					intersectionRayR[intersectCount++] = yMinR;
				}
			}
		}



		// Y Maximum Plane
		float yMaxR = (boxMax.getY()-yRs) / yRd;
		if (yMaxR >= 0.0f) {
			Vector3 yMaxPos = rayStart + rayDir * yMaxR;
			float yMaxdx = yMaxPos.getX() - xMin;
			if (yMaxdx >= 0.0f  &&  yMaxdx <= lenX) {
				float yMaxdz = yMaxPos.getZ() - zMin;
				if (yMaxdz >= 0.0f  &&  yMaxdz <= lenZ) {
					if (intersectCount == 2)
						return 1;

					intersectionPoints[intersectCount] = yMaxPos;
					intersectionRayR[intersectCount++] = yMaxR;
				}
			}
		}
	}

	if (zRd != 0.0f) {
		// Z Minimum Plane
		float zMinR = (boxMin.getZ()-zRs) / zRd;
		if (zMinR >= 0.0f) {
			Vector3 zMinPos = rayStart + rayDir * zMinR;
			float zMindx = zMinPos.getX() - xMin;
			if (zMindx >= 0.0f  &&  zMindx <= lenX) {
				float zMindy = zMinPos.getY() - yMin;
				if (zMindy >= 0.0f  &&  zMindy <= lenY) {
					if (intersectCount == 2)
						return 1;

					intersectionPoints[intersectCount] = zMinPos;
					intersectionRayR[intersectCount++] = zMinR;
				}
			}
		}

		// Z Maximum Plane
		float zMaxR = (boxMax.getZ()-zRs) / zRd;
		if (zMaxR >= 0.0f) {
			Vector3 zMaxPos = rayStart + rayDir * zMaxR;
			float zMaxdx = zMaxPos.getX() - xMin;
			if (zMaxdx >= 0.0f  &&  zMaxdx <= lenX) {
				float zMaxdy = zMaxPos.getY() - yMin;
				if (zMaxdy >= 0.0f  &&  zMaxdy <= lenY) {
					if (intersectCount == 2)
						return 1;

					intersectionPoints[intersectCount] = zMaxPos;
					intersectionRayR[intersectCount++] = zMaxR;
				}
			}
		}
	}

	return intersectCount;
}


bool IntersectRayBox (
		const Vector3& rayStart, const Vector3& rayDir,
		const Vector3& boxOrigin, const Vector3& boxExtX, const Vector3& boxExtY, const Vector3& boxExtZ,
		float& rMin, float& rMax
) {
	float rxMin, rxMax;
	float ryMin, ryMax;
	float rzMin, rzMax;
	float num1, num2;

	//bool xMissedInside, yMissedInside, zMissedInside;

	Vector3 xn = boxExtY.cross(boxExtZ);
	Vector3 yn = boxExtX.cross(boxExtZ);
	Vector3 zn = boxExtX.cross(boxExtY);

	// X Plane
	bool xIntersects = IntersectRayPlane(rayStart, rayDir, xn, boxOrigin, rxMin, num1);
	xIntersects = IntersectRayPlane(rayStart, rayDir, xn, boxOrigin+boxExtX, rxMax, num2)  ||  xIntersects;

	if (xIntersects) {
		rMin = min(rxMin, rxMax);
		rMax = max(rxMin, rxMax);
	} else {
		if (num1*num2 >= 0.0f) {
			// num1 and num2 have the same sign, meaning the ray missed the X plane pair from outside. In this
			// case, there is no way the ray could intersect the box
			return false;
		}
	}

	// Y Plane
	bool yIntersects = IntersectRayPlane(rayStart, rayDir, yn, boxOrigin, ryMin, num1);
	yIntersects = IntersectRayPlane(rayStart, rayDir, yn, boxOrigin+boxExtY, ryMax, num2)  ||  yIntersects;

	if (yIntersects) {
		if (xIntersects) {
			rMin = max(rMin, min(ryMin, ryMax));
			rMax = min(rMax, max(ryMin, ryMax));
		} else {
			rMin = min(ryMin, ryMax);
			rMax = max(ryMin, ryMax);
		}
	} else {
		if (num1*num2 >= 0.0f) {
			// num1 and num2 have the same sign, meaning the ray missed the Y plane pair from outside. In this
			// case, there is no way the ray could intersect the box
			return false;
		}
	}

	// Z Plane
	bool zIntersects = IntersectRayPlane(rayStart, rayDir, zn, boxOrigin, rzMin, num1);
	zIntersects = IntersectRayPlane(rayStart, rayDir, zn, boxOrigin+boxExtZ, rzMax, num2)  ||  zIntersects;

	if (zIntersects) {
		if (xIntersects  ||  yIntersects) {
			rMin = max(rMin, min(rzMin, rzMax));
			rMax = min(rMax, max(rzMin, rzMax));
		} else {
			rMin = min(rzMin, rzMax);
			rMax = max(rzMin, rzMax);
		}
	} else {
		if (num1*num2 >= 0.0f) {
			// num1 and num2 have the same sign, meaning the ray missed the Z plane pair from outside. In this
			// case, there is no way the ray could intersect the box
			return false;
		}
	}

	return rMin <= rMax  &&  rMax >= 0.0f;
}


bool IntersectRayTriangle (
		const Vector3& rayStart, const Vector3& rayDir,
		const Vector3& v0, const Vector3& v1, const Vector3& v2,
		float& s, float& t,
		float& rayR
) {
	Vector3 u = v1-v0;
	Vector3 v = v2-v0;
	Vector3 n = u.cross(v);

	float sideFactor;

	if (!IntersectRayPlane(rayStart, rayDir, n, v0, rayR, sideFactor))
		return false;

	Vector3 pos = rayStart + rayDir * rayR;
	Vector3 w = pos - v0;

	Vector3 ncu = n.cross(u);
	Vector3 ncv = n.cross(v);

	float uDotNcv = u.dot(ncv);
	float vDotNcu = v.dot(ncu);

	if (uDotNcv == 0.0f  ||  vDotNcu == 0.0f) {
		// Triangle is degenerate

		s = -1.0f;
		t = -1.0f;

		return false;
	}

	s = w.dot(ncv) / uDotNcv;
	t = w.dot(ncu) / vDotNcu;

	if (s < 0.0f  ||  t < 0.0f  ||  s+t > 1.0f) {
		return false;
	}

	return true;
}
