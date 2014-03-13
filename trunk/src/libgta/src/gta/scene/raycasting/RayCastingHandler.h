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

#ifndef RAYCASTINGHANDLER_H_
#define RAYCASTINGHANDLER_H_

#include <gta/config.h>
#include <nxcommon/math/Vector3.h>
#include <nxcommon/math/Matrix4.h>



class RayCastingHandler
{
public:
	enum ObjectDataFlags
	{
		ObjectDataFlagBoundingSphere = 1 << 0,
		ObjectDataFlagBoundingBox = 1 << 1,
	};

	struct ObjectData
	{
		uint32_t flags;

		Vector3 boundingSphereCenter;
		float boundingSphereRadius;

		Vector3 boundingBoxMin;
		Vector3 boundingBoxExtX;
		Vector3 boundingBoxExtY;
		Vector3 boundingBoxExtZ;

		Matrix4 modelMatrix;
	};

	struct SphereData
	{
		Vector3 center;
		float radius;
	};

	struct BoxData
	{
		Vector3 min;
		Vector3 max;
	};

	struct MeshData
	{
		uint32_t numVertices;
		uint32_t numFaces;

		float* vertices;
		uint32_t* indices;
	};

	struct SphereResult
	{
		Vector3 pointA;
		Vector3 pointB;
		float rayRa;
		float rayRb;
	};

	struct BoxResult
	{
		Vector3 intersectionPoints[2];
		float intersectionRayR[2];
	};

	struct MeshResult
	{
		uint32_t faceIndex;
		float s;
		float t;
		float rayR;
	};

public:
	virtual ~RayCastingHandler() {}

	virtual void startRayCasting(const Vector3& rayStart, const Vector3& rayDir, int flags) {}

	virtual bool nextObject(ObjectData& data, void*& userPtr) = 0;

	virtual bool loadObjectDetails(void* userPtr) { return true; }

	virtual bool nextSphere(void* objUserPtr, SphereData& data, void*& userPtr) { return false; }
	virtual bool nextBox(void* objUserPtr, BoxData& data, void*& userPtr) { return false; }
	virtual bool nextMesh(void* objUserPtr, MeshData& data, void*& userPtr) { return false; }

	virtual void sphereIntersectionReported(const SphereResult& res, void* objUserPtr, void* userPtr) {}
	virtual void boxIntersectionReported(const BoxResult& res, void* objUserPtr, void* userPtr) {}
	virtual void meshIntersectionReported(const MeshResult& res, void* objUserPtr, void* userPtr) {}

	virtual void finishObject(void* userPtr) {}

	virtual void finishRayCasting() {}
};

#endif /* RAYCASTINGHANDLER_H_ */
