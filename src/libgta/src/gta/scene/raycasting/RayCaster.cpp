/*
	Copyright 2010-2014 David "Alemarius Nexus" Lerch

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

#include "RayCaster.h"
#include <nxcommon/math/intersection.h>
#include <nxcommon/exception/InvalidStateException.h>
#include <nxcommon/util.h>
#include <list>
#include <set>
#include <vector>
#include <algorithm>

using std::list;
using std::vector;
using std::min;
using std::max;
using std::sort;
using std::partial_sort;
using std::set;



void RayCaster::setHandler(RayCastingHandler* handler)
{
	this->handler = handler;
}


void RayCaster::castRay(const Vector3& start, const Vector3& dir, int flags, uint32_t maxResults)
{
	if (!handler) {
		throw InvalidStateException("RayCaster::castRay() called without an active RayCastingHandler!",
				__FILE__, __LINE__);
	}

	Vector3 ndir = dir;
	ndir.normalize();

	if (maxResults == 0)
		maxResults = UINT32_MAX;

	uint32_t numResults = 0;

	vector<CastingResult> results;

	handler->startRayCasting(start, ndir, flags);

	void* userPtr;

	RayCastingHandler::ObjectData objData;

	uint32_t objID = 0;

	void* objUserPtr;
	while (handler->nextObject(objData, objUserPtr)) {
		bool objHasResults = false;

		if ((objData.flags & RayCastingHandler::ObjectDataFlagBoundingSphere)  !=  0) {
			if (!IntersectRaySphereSimple(start, ndir, objData.boundingSphereCenter, objData.boundingSphereRadius)) {
				handler->finishObject(objUserPtr);
				objID++;
				continue;
			}
		}
		if ((objData.flags & RayCastingHandler::ObjectDataFlagBoundingBox)  !=  0) {
			float rMin, rMax;
			bool intersects = IntersectRayBox (
					start, ndir,
					objData.boundingBoxMin, objData.boundingBoxExtX, objData.boundingBoxExtY, objData.boundingBoxExtZ,
					rMin, rMax
					);

			if (!intersects) {
				handler->finishObject(objUserPtr);
				objID++;
				continue;
			}
		}

		if (!handler->loadObjectDetails(objUserPtr)) {
			handler->finishObject(objUserPtr);
			objID++;
			continue;
		}

		RayCastingHandler::SphereData sphereData;
		while (numResults < maxResults  &&  handler->nextSphere(objUserPtr, sphereData, userPtr)) {
			if ((flags & CalculateIntersectionPosition)  !=  0  ||  (flags & Sorted)  !=  0) {
				Vector3 pointA, pointB;
				bool intersects = IntersectRaySphere (
						start, ndir,
						objData.modelMatrix * sphereData.center, sphereData.radius,
						pointA, pointB);

				if (intersects) {
					RayCastingHandler::SphereResult sres;
					sres.pointA = pointA;
					sres.pointB = pointB;
					sres.rayRa = (pointA - start).dot(ndir);
					sres.rayRb = (pointB - start).dot(ndir);

					if ((flags & Sorted)  !=  0) {
						CastingResult res;
						res.objID = objID;
						res.rayR = sres.rayRa;
						res.type = Sphere;
						res.objUserPtr = objUserPtr;
						res.userPtr = userPtr;
						res.sphereRes = sres;

						results.push_back(res);
					} else {
						handler->sphereIntersectionReported(sres, objUserPtr, userPtr);
						numResults++;
					}

					objHasResults = true;
				}
			} else {
				if (IntersectRaySphereSimple(start, ndir, objData.modelMatrix * sphereData.center, sphereData.radius)) {
					RayCastingHandler::SphereResult sres;

					handler->sphereIntersectionReported(sres, objUserPtr, userPtr);
					numResults++;

					objHasResults = true;
				}
			}
		}

		RayCastingHandler::BoxData boxData;
		while (numResults < maxResults  &&  handler->nextBox(objUserPtr, boxData, userPtr)) {
			Vector3 boxOrigin = boxData.min;
			Vector3 boxCornerX(boxData.max.getX(), boxOrigin.getY(), boxOrigin.getZ());
			Vector3 boxCornerY(boxOrigin.getX(), boxData.max.getY(), boxOrigin.getZ());
			Vector3 boxCornerZ(boxOrigin.getX(), boxOrigin.getY(), boxData.max.getZ());

			Vector3 wboxOrigin = objData.modelMatrix * boxOrigin;
			Vector3 wboxExtX = (objData.modelMatrix * boxCornerX) - wboxOrigin;
			Vector3 wboxExtY = (objData.modelMatrix * boxCornerY) - wboxOrigin;
			Vector3 wboxExtZ = (objData.modelMatrix * boxCornerZ) - wboxOrigin;

			float rMin, rMax;

			bool intersects = IntersectRayBox (
					start, ndir,
					wboxOrigin, wboxExtX, wboxExtY, wboxExtZ, rMin, rMax
					);

			if (intersects) {
				RayCastingHandler::BoxResult bres;

				bres.intersectionPoints[0] = start + dir*rMin;
				bres.intersectionPoints[1] = start + dir*rMax;

				bres.intersectionRayR[0] = rMin;
				bres.intersectionRayR[1] = rMax;

				if ((flags & Sorted)  !=  0) {
					CastingResult res;
					res.objID = objID;
					res.rayR = rMin;
					res.type = Box;
					res.objUserPtr = objUserPtr;
					res.userPtr = userPtr;
					res.boxRes = bres;

					results.push_back(res);
				} else {
					handler->boxIntersectionReported(bres, objUserPtr, userPtr);
					numResults++;
				}

				objHasResults = true;
			}
		}



		RayCastingHandler::MeshData meshData;
		while (handler->nextMesh(objUserPtr, meshData, userPtr)) {
			for (uint32_t i = 0 ; i < meshData.numFaces ; i++) {
				uint32_t idx0 = meshData.indices[i*3];
				uint32_t idx1 = meshData.indices[i*3 + 1];
				uint32_t idx2 = meshData.indices[i*3 + 2];

				float s, t;
				float rayR;

				Vector3 vtx0(meshData.vertices[idx0*3], meshData.vertices[idx0*3+1], meshData.vertices[idx0*3+2]);
				Vector3 vtx1(meshData.vertices[idx1*3], meshData.vertices[idx1*3+1], meshData.vertices[idx1*3+2]);
				Vector3 vtx2(meshData.vertices[idx2*3], meshData.vertices[idx2*3+1], meshData.vertices[idx2*3+2]);

				bool intersects = IntersectRayTriangle(start, ndir,
						objData.modelMatrix * vtx0,
						objData.modelMatrix * vtx1,
						objData.modelMatrix * vtx2,
						s, t, rayR
						);

				if (intersects) {
					RayCastingHandler::MeshResult mres;
					mres.faceIndex = i;
					mres.s = s;
					mres.t = t;
					mres.rayR = rayR;

					if ((flags & Sorted)  !=  0) {
						CastingResult res;
						res.objID = objID;
						res.rayR = rayR;
						res.type = Mesh;
						res.objUserPtr = objUserPtr;
						res.userPtr = userPtr;
						res.meshRes = mres;

						results.push_back(res);
					} else {
						handler->meshIntersectionReported(mres, objUserPtr, userPtr);
						numResults++;
					}

					objHasResults = true;
				}
			}
		}

		if ((flags & Sorted)  ==  0  ||  !objHasResults) {
			handler->finishObject(objUserPtr);
		}

		objID++;
	}

	if ((flags & Sorted)  !=  0) {
		CastingResultComparator comp;

		if (maxResults >= results.size()) {
			sort(results.begin(), results.end(), comp);
		} else {
			partial_sort(results.begin(), results.begin() + numResults, results.end(), comp);
		}

		set<uint32_t> finishedObjs;

		uint32_t i = 0;
		for (vector<CastingResult>::iterator it = results.begin() ; it != results.end()  &&  i < maxResults ; it++, i++) {
			CastingResult& res = *it;

			if (res.type == Sphere) {
				handler->sphereIntersectionReported(res.sphereRes, res.objUserPtr, res.userPtr);
			} else if (res.type == Box) {
				handler->boxIntersectionReported(res.boxRes, res.objUserPtr, res.userPtr);
			} else {
				handler->meshIntersectionReported(res.meshRes, res.objUserPtr, res.userPtr);
			}
		}

		for (vector<CastingResult>::iterator it = results.begin() ; it != results.end() ; it++) {
			CastingResult& res = *it;

			if (finishedObjs.insert(res.objID).second) {
				handler->finishObject(res.objUserPtr);
			}
		}
	}

	handler->finishRayCasting();
}
